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
badd +107 include/minidnn/modular_components.hpp
badd +19 Component/search_controller.cpp
badd +30 include/minidnn/Component/ParaController.hpp
badd +10 include/minidnn/Component/Worker.hpp
badd +25 Component/sgdthread.cpp
badd +212 elasync.cpp
badd +31 ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/NetworkExecutor.h
badd +21 ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/Optimizer.h
badd +201 ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/NetworkTopology.h
argglobal
%argdel
$argadd include/minidnn/modular_components.hpp
edit elasync.cpp
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
wincmd _ | wincmd |
split
1wincmd k
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
exe '2resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
exe '3resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
exe '4resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 4resize ' . ((&columns * 104 + 157) / 314)
exe '5resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 5resize ' . ((&columns * 104 + 157) / 314)
argglobal
balt ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/NetworkTopology.h
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
128
normal! zo
129
normal! zo
132
normal! zo
let s:l = 212 - ((49 * winheight(0) + 49) / 98)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 212
normal! 047|
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/modular_components.hpp", ":p")) | buffer include/minidnn/modular_components.hpp | else | edit include/minidnn/modular_components.hpp | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/modular_components.hpp
endif
balt elasync.cpp
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=0
setlocal fml=1
setlocal fdn=20
setlocal fen
1
normal! zo
4
normal! zo
11
normal! zo
15
normal! zo
25
normal! zo
35
normal! zo
44
normal! zo
55
normal! zo
66
normal! zo
73
normal! zo
78
normal! zo
85
normal! zo
94
normal! zo
96
normal! zo
106
normal! zo
110
normal! zo
115
normal! zo
let s:l = 107 - ((26 * winheight(0) + 24) / 49)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 107
normal! 033|
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/Component/Worker.hpp", ":p")) | buffer include/minidnn/Component/Worker.hpp | else | edit include/minidnn/Component/Worker.hpp | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/Component/Worker.hpp
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
6
normal! zo
let s:l = 23 - ((22 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 23
normal! 090|
wincmd w
argglobal
if bufexists(fnamemodify("Component/search_controller.cpp", ":p")) | buffer Component/search_controller.cpp | else | edit Component/search_controller.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/search_controller.cpp
endif
balt Component/sgdthread.cpp
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 19 - ((18 * winheight(0) + 24) / 49)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 19
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("Component/sgdthread.cpp", ":p")) | buffer Component/sgdthread.cpp | else | edit Component/sgdthread.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/sgdthread.cpp
endif
balt Component/search_controller.cpp
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
4
normal! zo
6
normal! zo
7
normal! zo
let s:l = 26 - ((25 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 26
normal! 031|
wincmd w
5wincmd w
exe 'vert 1resize ' . ((&columns * 104 + 157) / 314)
exe '2resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
exe '3resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
exe '4resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 4resize ' . ((&columns * 104 + 157) / 314)
exe '5resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 5resize ' . ((&columns * 104 + 157) / 314)
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
