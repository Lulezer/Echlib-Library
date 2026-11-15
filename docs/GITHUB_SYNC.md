# Syncing this folder to GitHub (`Lulezer/Echlib-Library`)

One-time setup from `Echlib (Current)`:

```powershell
cd "D:\Coding\C++ Libary Project\Echlib (Current)"
git init
git remote add origin https://github.com/Lulezer/Echlib-Library.git
git fetch origin
```

If `main` on GitHub has unrelated history (old include-only tree), either:

- **Replace remote** (you own the repo): force-push after review, or  
- **Fresh branch**: push `main-v2` and set it as default in GitHub settings.

```powershell
git add -A
git status   # confirm no .vs/ or out/ slipped in
git commit -m "Ship full CMake tree with thirdparty; refresh docs"
git push -u origin main
```

Tag a release when smoke-tested:

```powershell
git tag -a v1.3 -m "CMake-first repo; submodule-friendly"
git push origin v1.3
```

## Starter repo

From `echlib-starter/`:

```powershell
git init
git remote add origin https://github.com/Lulezer/echlib-starter.git
git submodule add https://github.com/Lulezer/Echlib-Library.git thirdparty/echlib
git add .
git commit -m "Minimal echlib consumer template"
git push -u origin main
```

Push **Echlib-Library first**, then add the submodule so the URL resolves.