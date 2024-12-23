### Make everything executable (recursively) just to be safe
`chmod -R +x ~/gbdk`

### Then remove the quarantine attribute recursively
`xattr -r -d com.apple.quarantine ~/gbdk 2>/dev/null`

---

- `.gb` is your playable ROM,
- `.map` helps you see memory usage and addresses,
- `.moi` is an internal/debugging artifact from the compiler/linker.
