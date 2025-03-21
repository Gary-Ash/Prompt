/*****************************************************************************************
 * segmentGit.cpp
 *
 * The file implements a git status for current directory segment
 *
 * Author   :  Gary Ash <gary.ash@icloud.com>
 * Created  :  20-Mar-2025 10:54pm
 * Modified :
 *
 * Copyright © 2024 By Gary Ash All rights reserved.
 ****************************************************************************************/

/*****************************************************************************************
 * include files
 ****************************************************************************************/
#include <git2.h>
#include <pwd.h>
#include <unistd.h>
#include "Prompt.h"

static void     gatherRemoteStats(git_repository* repo, git_reference* head, git_strarray* remotes);
static void     gatherLocalStats(git_repository*  repo);
static int      stashcb(size_t index, const char* message, const git_oid* stashID, void* payload);
static int      cred_acquire_cb(git_credential** out, const char* url, const char* username_from_url, unsigned int allowed_types, void* payload);

static bool			calledOnce = false;
static std::string	currentBranch;
static std::string	remoteHost;
static size_t		ahead[2]	= { 0, 0 };
static size_t		behind[2]	= { 0, 0 };
static size_t		modified	= 0;
static size_t		staged		= 0;
static size_t		conflicts	= 0;
static size_t		untracked	= 0;
static size_t		stashed		= 0;


void segmentGit(segmentOptions& options, renderSegment& rendered, int argc, const char** argv)
{
	char			dir[PATH_MAX];
	git_repository* repo = NULL;

	git_libgit2_init();
	getcwd(dir, PATH_MAX);
	int error = git_repository_open_ext(&repo, dir, 0, NULL);
	if (error == GIT_OK) {
		remoteHost = "";
		remoteHost.append(" ");
		gatherLocalStats(repo);
		git_reference* head = NULL;
		error = git_repository_head(&head, repo);
		if (error == GIT_OK) {
			git_strarray remotes = { 0 };
			currentBranch	= git_reference_shorthand(head);
			error			= git_remote_list(&remotes, repo);
			if (error == GIT_OK && remotes.count > 0) {
				gatherRemoteStats(repo, head, &remotes);
			}
			git_reference_free(head);
		}else if (error == GIT_EUNBORNBRANCH || error == GIT_ENOTFOUND) {
			currentBranch = "main";
		}
		git_repository_free(repo);
		std::string stats;
		if (ahead [0] != 0 || behind [0] != 0 || ahead [1] != 0 || behind [1] != 0 || staged != 0 || modified != 0 || conflicts != 0 || stashed != 0) {
			stats.append(" ");
			if (ahead [0] > 0 || behind [0] > 0) {
				stats.append(std::to_string(ahead [0]));
				stats.append(" ");
				stats.append(std::to_string(behind [0]));
				stats.append("");
			}
			if (ahead [1] > 0 || behind [1] > 0) {
				appendSpaceAsNeeded(stats);
				stats.append(" ");
				stats.append(std::to_string(ahead [1]));
				stats.append(" ");
				stats.append(std::to_string(behind [1]));
				stats.append("");
			}
			if (staged != 0 || modified != 0 || untracked != 0 || conflicts != 0 || stashed != 0) {
				appendSpaceAsNeeded(stats);
				if (staged > 0) {
					stats.append(std::to_string(staged));
					stats.append("");
				}
				if (modified > 0) {
					appendSpaceAsNeeded(stats);
					stats.append(std::to_string(modified));
					stats.append("");
				}
				if (untracked > 0) {
					appendSpaceAsNeeded(stats);
					stats.append(std::to_string(untracked));
					stats.append("");
				}
				if (conflicts > 0) {
					appendSpaceAsNeeded(stats);
					stats.append(std::to_string(conflicts));
					stats.append("");
				}
				if (stashed > 0) {
					appendSpaceAsNeeded(stats);
					stats.append(std::to_string(stashed));
					stats.append("");
				}
			}
			rendered.foregroundColor	= options.foregroundColor;
			rendered.backgroundColor	= options.backgroundColor;
		} else {
			rendered.foregroundColor	= options.alternateForegroundColor;
			rendered.backgroundColor	= options.alternateBackgroundColor;
			rendered.text.append("✓");

			if (untracked > 0) {
				stats.append(" ");
				appendSpaceAsNeeded(stats);
				stats.append(std::to_string(untracked));
				stats.append("");
			}
		}
		rendered.text.append(remoteHost);
		if (currentBranch.length() > 25) {
			currentBranch = currentBranch.substr(0, 24) + "…";
		}
		rendered.text.append(stats);
		appendSpaceAsNeeded(rendered.text);
		rendered.text.append(currentBranch);
	}
	git_libgit2_shutdown();
}

static void gatherRemoteStats(git_repository* repo, git_reference* head, git_strarray* remotes)
{
	git_remote*		remote		= NULL;
	const git_oid*	localCommit = git_reference_target(head);
	int				error		= git_remote_lookup(&remote, repo, remotes->strings [0]);

	if (error != GIT_OK) {
		return;
	}
	const char* url = git_remote_url(remote);
	if (strstr(url, "github.com")) {
		remoteHost = "";
	} else if (strstr(url, "gitlab.com")) {
		remoteHost = "";
	} else if (strstr(url, "bitbucket")) {
		remoteHost = "";
	}
	git_remote_free(remote);
	for (size_t i = 0; i < remotes->count; ++i) {
		error = git_remote_lookup(&remote, repo, remotes->strings[i]);
		if (error != GIT_OK) {
			return;
		}
		size_t						refs_count = 0;
		const git_remote_head**		refs;
		git_remote_connect_options	options;
		git_remote_connect_options_init(&options, GIT_REMOTE_CONNECT_OPTIONS_VERSION);
		calledOnce						= false;
		options.callbacks.credentials	= cred_acquire_cb;
		error							= git_remote_connect_ext(remote, GIT_DIRECTION_FETCH, &options);
		if (error != GIT_OK) {
			git_remote_free(remote);
			return;
		}

		error = git_remote_fetch(remote, NULL, NULL, NULL);
		if (error != GIT_OK) {
			git_remote_free(remote);
			return;
		}
		error = git_remote_ls(&refs, &refs_count, remote);
		if (error != GIT_OK) {
			git_remote_free(remote);
			return;
		}
		bool		foundBranch		= false;
		std::string targetBranch	= "refs/heads/";
		targetBranch.append(currentBranch);
		for (size_t index = 0; index < refs_count; ++index) {
			if (targetBranch == refs[index]->name) {
				foundBranch = true;
				git_graph_ahead_behind(&ahead[i],
									   &behind[i],
									   repo,
									   localCommit,
									   const_cast<const git_oid*>(&refs[index]->oid));
				break;
			}
		}
		if (foundBranch == false && refs[0] != NULL) {
			git_graph_ahead_behind(&ahead[i], &behind[i], repo, localCommit, const_cast<const git_oid*>(&refs[0]->oid));
		}
		git_remote_free(remote);
	}
}

static void gatherLocalStats(git_repository*  repo)
{
	git_status_list*	status;
	git_status_options	statusopt = GIT_STATUS_OPTIONS_INIT;

	statusopt.show	= GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
	statusopt.flags = GIT_STATUS_OPT_INCLUDE_UNMODIFIED | GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;
	const git_status_entry* entry;
	git_status_list_new(&status, repo, &statusopt);
	size_t					count = git_status_list_entrycount(status);
	for (int statusIndex = 0; statusIndex < count; ++statusIndex) {
		entry = git_status_byindex(status, statusIndex);
		if (entry->status == GIT_STATUS_CURRENT) {
			continue;
		}
		if ((entry->status & (GIT_STATUS_INDEX_NEW | GIT_STATUS_INDEX_MODIFIED)) != 0) {
			++staged;
		}
		if ((entry->status & GIT_STATUS_WT_MODIFIED) != 0) {
			++modified;
		}
		if (entry->status == GIT_STATUS_CONFLICTED) {
			++conflicts;
		}
		if ((entry->status & GIT_STATUS_WT_NEW) == GIT_STATUS_WT_NEW || entry->status == GIT_STATUS_IGNORED) {
			++untracked;
		}
	}
	git_stash_foreach(repo, stashcb, NULL);
	git_status_list_free(status);
}

static int stashcb(size_t index, const char* message, const git_oid* stashID, void* payload)
{
	++stashed;
	return 0;
}

static int cred_acquire_cb(git_credential** out, const char* url, const char* username_from_url, unsigned int allowed_types, void* payload)
{
	uid_t			uid			= geteuid();
	struct passwd*	pw			= getpwuid(uid);
	const char*		username	= (username_from_url != NULL ? username_from_url  : pw->pw_name);

	if (calledOnce == false && allowed_types & GIT_CREDENTIAL_SSH_KEY) {
		calledOnce = true;
		int rc = git_credential_ssh_key_from_agent(out, username_from_url);
		return rc;
	} else if (allowed_types & GIT_CREDENTIAL_USERNAME) {
		return git_credential_username_new(out, username);
	}
	return 0;
}

