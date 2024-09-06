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
badd +509 elasync.cpp
badd +170 include/minidnn/NetworkExecutor.h
badd +449 main.cpp
badd +128 include/minidnn/Executors.hpp
badd +38 ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/NetworkTopology.h
badd +222 ~/Documents/code/cpp/shared-memory-sgd/elasync2.cpp
badd +22 ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/Output.h
argglobal
%argdel
$argadd elasync.cpp
edit include/minidnn/NetworkExecutor.h
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
wincmd _ | wincmd |
vsplit
wincmd _ | wincmd |
vsplit
3wincmd h
wincmd w
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
exe 'vert 1resize ' . ((&columns * 106 + 213) / 426)
exe 'vert 2resize ' . ((&columns * 106 + 213) / 426)
exe 'vert 3resize ' . ((&columns * 106 + 213) / 426)
exe 'vert 4resize ' . ((&columns * 105 + 213) / 426)
argglobal
balt main.cpp
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
15
normal! zo
19
normal! zo
40
normal! zo
64
normal! zo
205
normal! zo
215
normal! zo
218
normal! zo
220
normal! zo
222
normal! zo
let s:l = 38 - ((37 * winheight(0) + 53) / 106)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 38
normal! 049|
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/Executors.hpp", ":p")) | buffer include/minidnn/Executors.hpp | else | edit include/minidnn/Executors.hpp | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/Executors.hpp
endif
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
14
normal! zo
30
normal! zo
56
normal! zo
61
normal! zo
66
normal! zo
101
normal! zo
122
normal! zo
let s:l = 128 - ((86 * winheight(0) + 53) / 106)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 128
normal! 033|
wincmd w
argglobal
if bufexists(fnamemodify("elasync.cpp", ":p")) | buffer elasync.cpp | else | edit elasync.cpp | endif
if &buftype ==# 'terminal'
  silent file elasync.cpp
endif
balt include/minidnn/NetworkExecutor.h
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
46
normal! zo
247
normal! zo
394
normal! zo
459
normal! zo
463
normal! zo
505
normal! zo
let s:l = 488 - ((62 * winheight(0) + 53) / 106)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 488
normal! 031|
wincmd w
argglobal
if bufexists(fnamemodify("~/Documents/code/cpp/shared-memory-sgd/include/minidnn/Output.h", ":p")) | buffer ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/Output.h | else | edit ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/Output.h | endif
if &buftype ==# 'terminal'
  silent file ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/Output.h
endif
balt ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/NetworkTopology.h
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
8
normal! zo
22
normal! zo
let s:l = 71 - ((70 * winheight(0) + 53) / 106)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 71
normal! 023|
wincmd w
4wincmd w
exe 'vert 1resize ' . ((&columns * 106 + 213) / 426)
exe 'vert 2resize ' . ((&columns * 106 + 213) / 426)
exe 'vert 3resize ' . ((&columns * 106 + 213) / 426)
exe 'vert 4resize ' . ((&columns * 105 + 213) / 426)
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
