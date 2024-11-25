let SessionLoad = 1
let s:so_save = &g:so | let s:siso_save = &g:siso | setg so=0 siso=0 | setl so=-1 siso=-1
let v:this_session=expand("<sfile>:p")
silent only
silent tabonly
cd ~/Documents/code/cpp/shared-memory-sgd
if expand('%') == '' && !&modified && line('$') <= 1 && getline(1) == ''
  let s:wipebuf = bufnr('%')
endif
let s:shortmess_save = &shortmess
if &shortmess =~ 'A'
  set shortmess=aoOA
else
  set shortmess=aoO
endif
badd +50 include/minidnn/Component/ParaController.hpp
badd +1 Component/window_controller.cpp
badd +0 include/minidnn/Component/Monitor.hpp
badd +0 include/minidnn/modular_components.hpp
argglobal
%argdel
edit include/minidnn/modular_components.hpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
wincmd _ | wincmd |
vsplit
2wincmd h
wincmd w
wincmd _ | wincmd |
split
1wincmd k
wincmd w
wincmd w
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe 'vert 1resize ' . ((&columns * 104 + 157) / 314)
exe '2resize ' . ((&lines * 20 + 42) / 85)
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
exe '3resize ' . ((&lines * 61 + 42) / 85)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
exe 'vert 4resize ' . ((&columns * 104 + 157) / 314)
argglobal
balt Component/window_controller.cpp
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 82 - ((27 * winheight(0) + 41) / 82)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 82
normal! 036|
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/Component/ParaController.hpp", ":p")) | buffer include/minidnn/Component/ParaController.hpp | else | edit include/minidnn/Component/ParaController.hpp | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/Component/ParaController.hpp
endif
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 31 - ((30 * winheight(0) + 10) / 20)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 31
normal! 033|
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/Component/Monitor.hpp", ":p")) | buffer include/minidnn/Component/Monitor.hpp | else | edit include/minidnn/Component/Monitor.hpp | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/Component/Monitor.hpp
endif
balt include/minidnn/Component/ParaController.hpp
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 45 - ((38 * winheight(0) + 30) / 61)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 45
normal! 029|
wincmd w
argglobal
if bufexists(fnamemodify("Component/window_controller.cpp", ":p")) | buffer Component/window_controller.cpp | else | edit Component/window_controller.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/window_controller.cpp
endif
balt include/minidnn/Component/ParaController.hpp
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 56 - ((40 * winheight(0) + 41) / 82)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 56
normal! 0
wincmd w
exe 'vert 1resize ' . ((&columns * 104 + 157) / 314)
exe '2resize ' . ((&lines * 20 + 42) / 85)
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
exe '3resize ' . ((&lines * 61 + 42) / 85)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
exe 'vert 4resize ' . ((&columns * 104 + 157) / 314)
tabnext 1
if exists('s:wipebuf') && len(win_findbuf(s:wipebuf)) == 0 && getbufvar(s:wipebuf, '&buftype') isnot# 'terminal'
  silent exe 'bwipe ' . s:wipebuf
endif
unlet! s:wipebuf
set winheight=1 winwidth=20
let &shortmess = s:shortmess_save
let &winminheight = s:save_winminheight
let &winminwidth = s:save_winminwidth
let s:sx = expand("<sfile>:p:r")."x.vim"
if filereadable(s:sx)
  exe "source " . fnameescape(s:sx)
endif
let &g:so = s:so_save | let &g:siso = s:siso_save
set hlsearch
nohlsearch
doautoall SessionLoadPost
unlet SessionLoad
" vim: set ft=vim :
