/*****************************************************************************************
 * segmentGit.cpp
 *
 * The file implements a git status for current directory segment
 *
 * Author   :  Gary Ash <gary.ash@icloud.com>
 * Created  :   2-Feb-2026  4:30pm
 * Modified :  11-Feb-2026  3:00pm
 *
 * Copyright © 2026 By Gary Ash All rights reserved.
 ****************************************************************************************/

/*****************************************************************************************
 * include files
 ****************************************************************************************/
#include <git2.h>
#include <unistd.h>
#include "Prompt.h"

static void gatherRemoteStats(git_repository* repo, git_reference* head, git_strarray* remotes);
static void gatherLocalStats(git_repository* repo);
static int  stashcb(size_t index, const char* message, const git_oid* stashID, void* payload);

static std::string	currentBranch;
static std::string	remoteHost;
static size_t		ahead[2]	= { 0, 0 };
static size_t		behind[2]	= { 0, 0 };
static size_t		modified	= 0;
static size_t		staged		= 0;
static size_t		conflicts	= 0;
static size_t		untracked	= 0;
static size_t		stashed		= 0;


void segmentGit(segmentOptions& options, renderSegment& rendered, int argc, const char** argv) {
	char			dir[PATH_MAX];
	git_repository* repo = NULL;

	ahead[0]	= 0;
	ahead[1]	= 0;
	behind[0]	= 0;
	behind[1]	= 0;
	modified	= 0;
	staged		= 0;
	conflicts	= 0;
	untracked	= 0;
	stashed		= 0;
	currentBranch.clear();
	remoteHost.clear();

	git_libgit2_init();
	getcwd(dir, PATH_MAX);
	int error = git_repository_open_ext(&repo, dir, 0, NULL);
	if (error == GIT_OK) {
		remoteHost = "\uE702";
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
			git_strarray_dispose(&remotes);
			git_reference_free(head);
		} else if (error == GIT_EUNBORNBRANCH || error == GIT_ENOTFOUND) {
			currentBranch = "main";
		}
		git_repository_free(repo);
		std::string stats;
		if (ahead[0] != 0 || behind[0] != 0 || ahead[1] != 0 || behind[1] != 0 || staged != 0 || modified != 0 || conflicts != 0 || stashed != 0) {
			stats.append(" ");
			if (ahead[0] > 0 || behind[0] > 0) {
				stats.append(std::to_string(ahead[0]));
				stats.append("\uF01B ");
				stats.append(std::to_string(behind[0]));
				stats.append("\uF01A");
			}
			if (ahead[1] > 0 || behind[1] > 0) {
				appendSpaceAsNeeded(stats);
				stats.append(" ");
				stats.append(std::to_string(ahead[1]));
				stats.append("\uF01B ");
				stats.append(std::to_string(behind[1]));
				stats.append("\uF01A");
			}
			if (staged != 0 || modified != 0 || untracked != 0 || conflicts != 0 || stashed != 0) {
				appendSpaceAsNeeded(stats);
				if (staged > 0) {
					stats.append(std::to_string(staged));
					stats.append("\uF055");
				}
				if (modified > 0) {
					appendSpaceAsNeeded(stats);
					stats.append(std::to_string(modified));
					stats.append("\uF06A");
				}
				if (untracked > 0) {
					appendSpaceAsNeeded(stats);
					stats.append(std::to_string(untracked));
					stats.append("\uF059");
				}
				if (conflicts > 0) {
					appendSpaceAsNeeded(stats);
					stats.append(std::to_string(conflicts));
					stats.append("\uF057");
				}
				if (stashed > 0) {
					appendSpaceAsNeeded(stats);
					stats.append(std::to_string(stashed));
					stats.append("\uF01C");
				}
			}
			rendered.foregroundColor	= options.foregroundColor;
			rendered.backgroundColor	= options.backgroundColor;
		} else {
			rendered.foregroundColor	= options.alternateForegroundColor;
			rendered.backgroundColor	= options.alternateBackgroundColor;
			rendered.text.append("\u2713");

			if (untracked > 0) {
				stats.append(" ");
				appendSpaceAsNeeded(stats);
				stats.append(std::to_string(untracked));
				stats.append("\uF059");
			}
		}
		rendered.text.append(remoteHost);
		if (currentBranch.length() > 25) {
			currentBranch = currentBranch.substr(0, 24) + "\u2026";
		}
		rendered.text.append(stats);
		appendSpaceAsNeeded(rendered.text);
		rendered.text.append(currentBranch);
	}
	git_libgit2_shutdown();
}

static void gatherRemoteStats(git_repository* repo, git_reference* head, git_strarray* remotes) {
	git_remote*		remote		= NULL;
	const git_oid*	localOid	= git_reference_target(head);

	if (localOid == NULL) {
		return;
	}

	int error = git_remote_lookup(&remote, repo, remotes->strings[0]);
	if (error == GIT_OK) {
		const char* url = git_remote_url(remote);
		if (url != NULL) {
			if (strstr(url, "github.com")) {
				remoteHost = "\uF09B";
			} else if (strstr(url, "gitlab.com")) {
				remoteHost = "\uF296";
			} else if (strstr(url, "bitbucket")) {
				remoteHost = "\uF171";
			}
		}
		git_remote_free(remote);
	}

	size_t count = remotes->count > 2 ? 2 : remotes->count;
	for (size_t i = 0; i < count; ++i) {
		std::string refName = "refs/remotes/";
		refName.append(remotes->strings[i]);
		refName.append("/");
		refName.append(currentBranch);

		git_reference* remoteRef = NULL;
		if (git_reference_lookup(&remoteRef, repo, refName.c_str()) == GIT_OK) {
			const git_oid* remoteOid = git_reference_target(remoteRef);
			if (remoteOid != NULL) {
				git_graph_ahead_behind(&ahead[i], &behind[i], repo, localOid, remoteOid);
			}
			git_reference_free(remoteRef);
		}
	}
}

static void gatherLocalStats(git_repository* repo) {
	git_status_list*	status;
	git_status_options	statusopt = GIT_STATUS_OPTIONS_INIT;

	statusopt.show	= GIT_STATUS_SHOW_INDEX_AND_WORKDIR;
	statusopt.flags = GIT_STATUS_OPT_INCLUDE_UNTRACKED | GIT_STATUS_OPT_RECURSE_UNTRACKED_DIRS;
	const git_status_entry* entry;
	git_status_list_new(&status, repo, &statusopt);
	size_t count = git_status_list_entrycount(status);
	for (size_t statusIndex = 0; statusIndex < count; ++statusIndex) {
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
		if ((entry->status & GIT_STATUS_WT_NEW) == GIT_STATUS_WT_NEW) {
			++untracked;
		}
	}
	git_stash_foreach(repo, stashcb, NULL);
	git_status_list_free(status);
}

static int stashcb(size_t index, const char* message, const git_oid* stashID, void* payload) {
	++stashed;
	return 0;
}
