# Prompt

A fast, customizable Powerline-style shell prompt for macOS written in C++.

![Screenshot](documentation/ScreenShot.png)

## Features

- Powerline-style segmented prompt with arrow separators
- Git integration via libgit2 (no shell subprocesses)
  - Branch name with remote host icons (GitHub, GitLab, Bitbucket)
  - Ahead/behind counts for multiple remotes
  - Staged, modified, untracked, conflicted, and stashed file counts
  - Clean repository indicator
- Current working directory with SF Symbols for known folders (Desktop, Documents, Developer, Downloads, etc.)
- Username with alternate colors for root
- Hostname display
- Last command error status (symbolic errno names or simple indicator)
- Support for both zsh and bash
- True color (24-bit RGB) and 256-color palette support

## Requirements

- macOS
- C++17 compiler (clang or gcc)
- [libgit2](https://libgit2.org/)
- A [Nerd Font](https://www.nerdfonts.com/) for icons

## Building

### Install libgit2

Using Homebrew:

```bash
brew install libgit2
```

Or build from source:

```bash
./scripts/build-libgit2.sh
```

### Compile

```bash
make
```

To use gcc instead of clang:

```bash
make CXX=g++
```

## Installation

1. Copy the `prompt` binary to a directory in your PATH (e.g., `/usr/local/bin`)

2. Configure your shell:

**For zsh** (add to `~/.zshrc`):

```zsh
precmd() {
    PROMPT="$(prompt --error $?)"
}
```

**For bash** (add to `~/.bashrc`):

```bash
PROMPT_COMMAND='PS1="$(prompt --error $?)"'
```

## Configuration

The prompt is configured via environment variables. Set one of the following in your shell configuration:

- `SNAZZY_PROMPT` - 256-color palette configuration
- `SNAZZY_PROMPT_TRUE` - True color (24-bit RGB) configuration (used when `COLORTERM=truecolor` or `COLORTERM=24bit`)

### Format

```
segment,fg,bg[,alt_fg,alt_bg]:segment2,fg,bg[,alt_fg,alt_bg]:...
```

- Colors are specified as either:
  - A single number (0-255) for 256-color palette
  - Three semicolon-separated values (R;G;B) for true color
- Alternate colors are optional and used for special states (e.g., root user, read-only directory, clean git repo)

### Available Segments

| Segment   | Description |
|-----------|-------------|
| `user`    | Current username (alternate colors when root) |
| `machine` | Hostname |
| `cwd`     | Current working directory with folder icons (alternate colors when read-only) |
| `git`     | Git repository status (alternate colors when clean) |
| `err`     | Last command error as symbolic name (EPERM, ENOENT, etc.) |
| `short`   | Simple error indicator icon |

### Example Configurations

**256-color:**

```bash
export SNAZZY_PROMPT="cwd,255,166,255,196:git,255,200,255,35:short,255,1"
```

**True color:**

```bash
export SNAZZY_PROMPT_TRUE="user,255;255;255,106;168;214,255;255;255,106;168;214:machine,255;255;255,180;167;214:cwd,255;255;255,255;148;0,255;255;255,1:git,255;255;255,147;196;124,255;255;255,255;142;198:short,255;255;255,128;0;0"
```

## License

MIT License - see [LICENSE.markdown](LICENSE.markdown)

## Author

Gary Ash <gary.ash@icloud.com>
