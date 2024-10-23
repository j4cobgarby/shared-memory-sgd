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
badd +40 Component/sgdthread.cpp
badd +119 include/minidnn/modular_components.hpp
badd +36 Component/model_interface.cpp
badd +9 include/minidnn/Component/ModelInterfaces.hpp
badd +16 include/minidnn/Component/ParaController.hpp
badd +8 Component/search_controller.cpp
badd +10 include/minidnn/Component/Dispatcher.hpp
badd +24 Component/asyncdispatcher.cpp
badd +21 include/minidnn/Component/Monitor.hpp
badd +26 Component/windowmonitor.cpp
badd +1 main.cpp
argglobal
%argdel
$argadd Component/sgdthread.cpp
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabnew +setlocal\ bufhidden=wipe
tabrewind
edit Component/sgdthread.cpp
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
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
exe '3resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
exe '4resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 4resize ' . ((&columns * 104 + 157) / 314)
argglobal
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 37 - ((36 * winheight(0) + 49) / 98)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 37
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/modular_components.hpp", ":p")) | buffer include/minidnn/modular_components.hpp | else | edit include/minidnn/modular_components.hpp | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/modular_components.hpp
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
1
normal! zo
11
normal! zo
15
normal! zo
75
normal! zo
105
normal! zo
let s:l = 119 - ((37 * winheight(0) + 49) / 98)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 119
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/Component/ModelInterfaces.hpp", ":p")) | buffer include/minidnn/Component/ModelInterfaces.hpp | else | edit include/minidnn/Component/ModelInterfaces.hpp | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/Component/ModelInterfaces.hpp
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
1
normal! zo
6
normal! zo
let s:l = 10 - ((8 * winheight(0) + 24) / 49)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 10
normal! 075|
wincmd w
argglobal
if bufexists(fnamemodify("Component/model_interface.cpp", ":p")) | buffer Component/model_interface.cpp | else | edit Component/model_interface.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/model_interface.cpp
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
7
normal! zo
9
normal! zo
14
normal! zo
let s:l = 13 - ((12 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 13
normal! 030|
wincmd w
exe 'vert 1resize ' . ((&columns * 104 + 157) / 314)
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
exe '3resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
exe '4resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 4resize ' . ((&columns * 104 + 157) / 314)
tabnext
edit include/minidnn/Component/ParaController.hpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
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
exe '1resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 1resize ' . ((&columns * 156 + 157) / 314)
exe '2resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 2resize ' . ((&columns * 156 + 157) / 314)
exe 'vert 3resize ' . ((&columns * 157 + 157) / 314)
argglobal
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
5
normal! zo
let s:l = 21 - ((20 * winheight(0) + 24) / 49)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 21
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/modular_components.hpp", ":p")) | buffer include/minidnn/modular_components.hpp | else | edit include/minidnn/modular_components.hpp | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/modular_components.hpp
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
1
normal! zo
11
normal! zo
15
normal! zo
75
normal! zo
105
normal! zo
let s:l = 47 - ((7 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 47
normal! 02|
wincmd w
argglobal
if bufexists(fnamemodify("Component/search_controller.cpp", ":p")) | buffer Component/search_controller.cpp | else | edit Component/search_controller.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/search_controller.cpp
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
let s:l = 8 - ((7 * winheight(0) + 49) / 98)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 8
normal! 0
wincmd w
exe '1resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 1resize ' . ((&columns * 156 + 157) / 314)
exe '2resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 2resize ' . ((&columns * 156 + 157) / 314)
exe 'vert 3resize ' . ((&columns * 157 + 157) / 314)
tabnext
edit include/minidnn/modular_components.hpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
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
exe 'vert 1resize ' . ((&columns * 156 + 157) / 314)
exe '2resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 2resize ' . ((&columns * 157 + 157) / 314)
exe '3resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 3resize ' . ((&columns * 157 + 157) / 314)
argglobal
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
11
normal! zo
15
normal! zo
75
normal! zo
105
normal! zo
let s:l = 149 - ((71 * winheight(0) + 49) / 98)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 149
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/Component/Dispatcher.hpp", ":p")) | buffer include/minidnn/Component/Dispatcher.hpp | else | edit include/minidnn/Component/Dispatcher.hpp | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/Component/Dispatcher.hpp
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
1
normal! zo
6
normal! zo
let s:l = 12 - ((11 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 12
normal! 024|
wincmd w
argglobal
if bufexists(fnamemodify("Component/asyncdispatcher.cpp", ":p")) | buffer Component/asyncdispatcher.cpp | else | edit Component/asyncdispatcher.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/asyncdispatcher.cpp
endif
balt include/minidnn/Component/Dispatcher.hpp
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
5
normal! zo
13
normal! zo
let s:l = 24 - ((23 * winheight(0) + 24) / 49)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 24
normal! 03|
wincmd w
exe 'vert 1resize ' . ((&columns * 156 + 157) / 314)
exe '2resize ' . ((&lines * 48 + 50) / 101)
exe 'vert 2resize ' . ((&columns * 157 + 157) / 314)
exe '3resize ' . ((&lines * 49 + 50) / 101)
exe 'vert 3resize ' . ((&columns * 157 + 157) / 314)
tabnext
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
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
argglobal
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
11
normal! zo
15
normal! zo
75
normal! zo
105
normal! zo
let s:l = 136 - ((56 * winheight(0) + 49) / 98)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 136
normal! 010|
wincmd w
argglobal
if bufexists(fnamemodify("Component/windowmonitor.cpp", ":p")) | buffer Component/windowmonitor.cpp | else | edit Component/windowmonitor.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/windowmonitor.cpp
endif
balt include/minidnn/Component/Monitor.hpp
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
10
normal! zo
let s:l = 26 - ((25 * winheight(0) + 49) / 98)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 26
normal! 060|
wincmd w
argglobal
if bufexists(fnamemodify("include/minidnn/Component/Monitor.hpp", ":p")) | buffer include/minidnn/Component/Monitor.hpp | else | edit include/minidnn/Component/Monitor.hpp | endif
if &buftype ==# 'terminal'
  silent file include/minidnn/Component/Monitor.hpp
endif
balt Component/windowmonitor.cpp
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
let s:l = 21 - ((20 * winheight(0) + 49) / 98)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 21
normal! 014|
wincmd w
exe 'vert 1resize ' . ((&columns * 104 + 157) / 314)
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
tabnext
edit main.cpp
let s:save_splitbelow = &splitbelow
let s:save_splitright = &splitright
set splitbelow splitright
wincmd _ | wincmd |
vsplit
1wincmd h
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
exe 'vert 1resize ' . ((&columns * 156 + 157) / 314)
exe 'vert 2resize ' . ((&columns * 157 + 157) / 314)
argglobal
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 1 - ((0 * winheight(0) + 49) / 98)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("main.cpp", ":p")) | buffer main.cpp | else | edit main.cpp | endif
if &buftype ==# 'terminal'
  silent file main.cpp
endif
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 1 - ((0 * winheight(0) + 49) / 98)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
wincmd w
exe 'vert 1resize ' . ((&columns * 156 + 157) / 314)
exe 'vert 2resize ' . ((&columns * 157 + 157) / 314)
tabnext 5
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
