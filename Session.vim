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
badd +149 include/minidnn/modular_components.hpp
badd +7 Component/windowmonitor.cpp
badd +10 Component/systemexecutor.cpp
badd +1 local_experiments/03-09-2024_10-47-12_LENET_ELASYNC_CIFAR10_.json
badd +35 Component/simplebatchcontroller.cpp
badd +22 main.cpp
badd +15 Component/asyncdispatcher.cpp
badd +26 include/minidnn/Component/Worker.hpp
badd +19 include/minidnn/Component/Monitor.hpp
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
exe '1resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 1resize ' . ((&columns * 104 + 157) / 314)
exe '2resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
exe '3resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
exe '4resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 4resize ' . ((&columns * 104 + 157) / 314)
exe '5resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 5resize ' . ((&columns * 104 + 157) / 314)
exe '6resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 6resize ' . ((&columns * 104 + 157) / 314)
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
136
normal! zo
145
normal! zo
let s:l = 165 - ((58 * winheight(0) + 24) / 49)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 165
normal! 032|
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
let s:l = 42 - ((8 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 42
normal! 010|
wincmd w
argglobal
if bufexists(fnamemodify("Component/sgdthread.cpp", ":p")) | buffer Component/sgdthread.cpp | else | edit Component/sgdthread.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/sgdthread.cpp
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
4
normal! zo
6
normal! zo
17
normal! zo
let s:l = 51 - ((42 * winheight(0) + 24) / 49)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 51
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("Component/windowmonitor.cpp", ":p")) | buffer Component/windowmonitor.cpp | else | edit Component/windowmonitor.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/windowmonitor.cpp
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
6
normal! zo
12
normal! zo
let s:l = 7 - ((6 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 7
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("Component/systemexecutor.cpp", ":p")) | buffer Component/systemexecutor.cpp | else | edit Component/systemexecutor.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/systemexecutor.cpp
endif
balt local_experiments/03-09-2024_10-47-12_LENET_ELASYNC_CIFAR10_.json
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 18 - ((17 * winheight(0) + 24) / 49)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 18
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("Component/asyncdispatcher.cpp", ":p")) | buffer Component/asyncdispatcher.cpp | else | edit Component/asyncdispatcher.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/asyncdispatcher.cpp
endif
balt include/minidnn/Component/Worker.hpp
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
3
normal! zo
13
normal! zo
let s:l = 18 - ((17 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 18
normal! 027|
wincmd w
3wincmd w
exe '1resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 1resize ' . ((&columns * 104 + 157) / 314)
exe '2resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
exe '3resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
exe '4resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 4resize ' . ((&columns * 104 + 157) / 314)
exe '5resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 5resize ' . ((&columns * 104 + 157) / 314)
exe '6resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 6resize ' . ((&columns * 104 + 157) / 314)
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
