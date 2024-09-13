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
badd +261 elasync.cpp
badd +42 include/minidnn/NetworkExecutor.h
badd +49 include/minidnn/Executors.hpp
badd +38 ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/NetworkTopology.h
badd +222 ~/Documents/code/cpp/shared-memory-sgd/elasync2.cpp
badd +58 ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/Output.h
badd +24 elastic_ctrl.cpp
argglobal
%argdel
$argadd elasync.cpp
edit include/minidnn/Executors.hpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
wincmd _ | wincmd |
vsplit
2wincmd h
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
exe 'vert 1resize ' . ((&columns * 106 + 160) / 320)
exe 'vert 2resize ' . ((&columns * 106 + 160) / 320)
exe 'vert 3resize ' . ((&columns * 106 + 160) / 320)
argglobal
balt ~/Documents/code/cpp/shared-memory-sgd/elasync2.cpp
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
17
normal! zo
21
normal! zo
55
normal! zo
56
normal! zo
109
normal! zo
114
normal! zo
157
normal! zo
184
normal! zo
let s:l = 162 - ((64 * winheight(0) + 40) / 81)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 162
normal! 049|
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/NetworkExecutor.h", ":p")) | buffer include/minidnn/NetworkExecutor.h | else | edit include/minidnn/NetworkExecutor.h | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/NetworkExecutor.h
endif
balt include/minidnn/Executors.hpp
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
14
normal! zo
let s:l = 42 - ((41 * winheight(0) + 40) / 81)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 42
normal! 025|
wincmd w
argglobal
if bufexists(fnamemodify("elastic_ctrl.cpp", ":p")) | buffer elastic_ctrl.cpp | else | edit elastic_ctrl.cpp | endif
if &buftype ==# 'terminal'
  silent file elastic_ctrl.cpp
endif
balt elasync.cpp
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
19
normal! zo
let s:l = 22 - ((21 * winheight(0) + 40) / 81)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 22
normal! 012|
wincmd w
3wincmd w
exe 'vert 1resize ' . ((&columns * 106 + 160) / 320)
exe 'vert 2resize ' . ((&columns * 106 + 160) / 320)
exe 'vert 3resize ' . ((&columns * 106 + 160) / 320)
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
