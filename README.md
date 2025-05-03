# demos

### xmake
```sh
# Configure package search path to handle weak network environment
xmake g --pkg_searchdirs="/download/packages"
# format
xmake ai_format -a
# generate compile_commands.json
xmake project -k compile_commands --lsp=clangd
```
