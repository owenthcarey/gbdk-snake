### Contributing to gbdk-snake

This repo uses Conventional Commits for all commits. Keep it simple: we do not use scopes.

## Conventional Commits

Use the form:

```
<type>: <subject>

[optional body]

[optional footer(s)]
```

Subject rules:

- Imperative mood, no trailing period, ≤ 72 characters
- UTF-8 allowed; avoid emoji in the subject

Accepted types:

- `build` – build system or external dependencies (e.g., CMake, GBDK toolchain)
- `chore` – maintenance (no game behavior change)
- `ci` – continuous integration configuration (workflows, Pages deploy)
- `docs` – documentation only
- `feat` – user-facing feature or capability
- `fix` – bug fix
- `perf` – performance improvements
- `refactor` – code change that neither fixes a bug nor adds a feature
- `revert` – revert of a previous commit
- `style` – formatting/whitespace (no code behavior)
- `test` – add/adjust tests only

Examples:

```text
feat: add pause menu to gameplay
fix: correct sprite flicker on title screen
docs: add build instructions for macOS and Linux
style: format C files and normalize whitespace
chore: update .gitignore for Game Boy artifacts
ci: build ROM and deploy GitHub Pages site
```

Breaking changes:

- Use `!` after the type or a `BREAKING CHANGE:` footer.

```text
feat!: change SRAM save format to new layout

BREAKING CHANGE: previous save files are incompatible with this version.
```
