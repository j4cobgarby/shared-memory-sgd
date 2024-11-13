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
badd +47 Component/sgdthread.cpp
badd +107 include/minidnn/modular_components.hpp
badd +26 Component/windowmonitor.cpp
badd +18 Component/systemexecutor.cpp
badd +0 local_experiments/03-09-2024_10-47-12_LENET_ELASYNC_CIFAR10_.json
badd +35 Component/simplebatchcontroller.cpp
badd +22 main.cpp
badd +27 Component/asyncdispatcher.cpp
badd +14 include/minidnn/Component/Worker.hpp
badd +12 include/minidnn/Component/Monitor.hpp
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
wincmd _ | wincmd |
split
1wincmd k
wincmd w
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
exe '1resize ' . ((&lines * 42 + 44) / 88)
exe 'vert 1resize ' . ((&columns * 94 + 142) / 284)
exe '2resize ' . ((&lines * 42 + 44) / 88)
exe 'vert 2resize ' . ((&columns * 94 + 142) / 284)
exe 'vert 3resize ' . ((&columns * 94 + 142) / 284)
exe '4resize ' . ((&lines * 42 + 44) / 88)
exe 'vert 4resize ' . ((&columns * 94 + 142) / 284)
exe '5resize ' . ((&lines * 42 + 44) / 88)
exe 'vert 5resize ' . ((&columns * 94 + 142) / 284)
argglobal
balt Component/sgdthread.cpp
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
12
normal! zo
59
normal! zo
100
normal! zo
118
normal! zo
120
normal! zo
120
normal! zc
let s:l = 68 - ((56 * winheight(0) + 21) / 42)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 68
normal! 018|
wincmd w
argglobal
if bufexists(fnamemodify("main.cpp", ":p")) | buffer main.cpp | else | edit main.cpp | endif
if &buftype ==# 'terminal'
  silent file main.cpp
endif
balt include/minidnn/modular_components.hpp
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
21
normal! zo
let s:l = 61 - ((25 * winheight(0) + 21) / 42)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 61
normal! 032|
wincmd w
argglobal
if bufexists(fnamemodify("Component/sgdthread.cpp", ":p")) | buffer Component/sgdthread.cpp | else | edit Component/sgdthread.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/sgdthread.cpp
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
4
normal! zo
6
normal! zo
17
normal! zo
let s:l = 37 - ((27 * winheight(0) + 42) / 85)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 37
normal! 016|
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/Component/Worker.hpp", ":p")) | buffer include/minidnn/Component/Worker.hpp | else | edit include/minidnn/Component/Worker.hpp | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/Component/Worker.hpp
endif
balt Component/asyncdispatcher.cpp
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
8
normal! zo
10
normal! zo
let s:l = 14 - ((10 * winheight(0) + 21) / 42)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 14
normal! 026|
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/Component/Monitor.hpp", ":p")) | buffer include/minidnn/Component/Monitor.hpp | else | edit include/minidnn/Component/Monitor.hpp | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/Component/Monitor.hpp
endif
balt Component/simplebatchcontroller.cpp
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
1
normal! zo
6
normal! zo
let s:l = 12 - ((11 * winheight(0) + 21) / 42)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 12
normal! 019|
wincmd w
3wincmd w
exe '1resize ' . ((&lines * 42 + 44) / 88)
exe 'vert 1resize ' . ((&columns * 94 + 142) / 284)
exe '2resize ' . ((&lines * 42 + 44) / 88)
exe 'vert 2resize ' . ((&columns * 94 + 142) / 284)
exe 'vert 3resize ' . ((&columns * 94 + 142) / 284)
exe '4resize ' . ((&lines * 42 + 44) / 88)
exe 'vert 4resize ' . ((&columns * 94 + 142) / 284)
exe '5resize ' . ((&lines * 42 + 44) / 88)
exe 'vert 5resize ' . ((&columns * 94 + 142) / 284)
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
