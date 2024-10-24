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
badd +17 Component/sgdthread.cpp
badd +61 include/minidnn/modular_components.hpp
badd +36 Component/model_interface.cpp
badd +9 include/minidnn/Component/ModelInterfaces.hpp
badd +1 include/minidnn/Component/ParaController.hpp
badd +8 Component/search_controller.cpp
badd +10 include/minidnn/Component/Dispatcher.hpp
badd +23 Component/asyncdispatcher.cpp
badd +21 include/minidnn/Component/Monitor.hpp
badd +21 Component/windowmonitor.cpp
badd +65 main.cpp
badd +0 main-old.cpp
badd +11 include/minidnn/Component/BatchController.hpp
badd +68 Component/simplebatchcontroller.cpp
badd +62 elasync.cpp
badd +77 ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/NetworkExecutor.h
badd +13 ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/RNG.h
badd +31 include/minidnn/Utils/Random.h
argglobal
%argdel
$argadd Component/sgdthread.cpp
tabnew +setlocal\ bufhidden=wipe
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
exe '3resize ' . ((&lines * 48 + 50) / 100)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
exe '4resize ' . ((&lines * 48 + 50) / 100)
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
let s:l = 17 - ((16 * winheight(0) + 48) / 97)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 17
normal! 037|
wincmd w
argglobal
if bufexists(fnamemodify("Component/asyncdispatcher.cpp", ":p")) | buffer Component/asyncdispatcher.cpp | else | edit Component/asyncdispatcher.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/asyncdispatcher.cpp
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
let s:l = 23 - ((22 * winheight(0) + 48) / 97)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 23
normal! 023|
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
let s:l = 10 - ((8 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 10
normal! 01|
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
let s:l = 13 - ((12 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 13
normal! 030|
wincmd w
2wincmd w
exe 'vert 1resize ' . ((&columns * 104 + 157) / 314)
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
exe '3resize ' . ((&lines * 48 + 50) / 100)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
exe '4resize ' . ((&lines * 48 + 50) / 100)
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
exe '1resize ' . ((&lines * 48 + 50) / 100)
exe 'vert 1resize ' . ((&columns * 156 + 157) / 314)
exe '2resize ' . ((&lines * 48 + 50) / 100)
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
let s:l = 21 - ((20 * winheight(0) + 24) / 48)
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
let s:l = 47 - ((8 * winheight(0) + 24) / 48)
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
let s:l = 8 - ((7 * winheight(0) + 48) / 97)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 8
normal! 0
wincmd w
exe '1resize ' . ((&lines * 48 + 50) / 100)
exe 'vert 1resize ' . ((&columns * 156 + 157) / 314)
exe '2resize ' . ((&lines * 48 + 50) / 100)
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
exe '2resize ' . ((&lines * 47 + 50) / 100)
exe 'vert 2resize ' . ((&columns * 157 + 157) / 314)
exe '3resize ' . ((&lines * 49 + 50) / 100)
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
let s:l = 149 - ((70 * winheight(0) + 48) / 97)
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
let s:l = 12 - ((11 * winheight(0) + 23) / 47)
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
exe '2resize ' . ((&lines * 47 + 50) / 100)
exe 'vert 2resize ' . ((&columns * 157 + 157) / 314)
exe '3resize ' . ((&lines * 49 + 50) / 100)
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
let s:l = 135 - ((54 * winheight(0) + 48) / 97)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 135
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
let s:l = 21 - ((20 * winheight(0) + 48) / 97)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 21
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
let s:l = 21 - ((20 * winheight(0) + 48) / 97)
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
edit main-old.cpp
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
balt main.cpp
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
let s:l = 476 - ((49 * winheight(0) + 48) / 97)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 476
normal! 016|
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
let s:l = 1 - ((0 * winheight(0) + 48) / 97)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 1
normal! 0
wincmd w
exe 'vert 1resize ' . ((&columns * 156 + 157) / 314)
exe 'vert 2resize ' . ((&columns * 157 + 157) / 314)
tabnext
edit include/minidnn/Component/BatchController.hpp
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
let &splitbelow = s:save_splitbelow
let &splitright = s:save_splitright
wincmd t
let s:save_winminheight = &winminheight
let s:save_winminwidth = &winminwidth
set winminheight=0
set winheight=1
set winminwidth=0
set winwidth=1
exe '1resize ' . ((&lines * 48 + 50) / 100)
exe 'vert 1resize ' . ((&columns * 104 + 157) / 314)
exe '2resize ' . ((&lines * 48 + 50) / 100)
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
exe 'vert 3resize ' . ((&columns * 104 + 157) / 314)
exe 'vert 4resize ' . ((&columns * 104 + 157) / 314)
argglobal
balt ~/Documents/code/cpp/shared-memory-sgd/include/minidnn/RNG.h
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
7
normal! zo
let s:l = 21 - ((20 * winheight(0) + 24) / 48)
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
balt include/minidnn/Component/BatchController.hpp
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
let s:l = 26 - ((25 * winheight(0) + 24) / 48)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 26
normal! 0
wincmd w
argglobal
if bufexists(fnamemodify("Component/simplebatchcontroller.cpp", ":p")) | buffer Component/simplebatchcontroller.cpp | else | edit Component/simplebatchcontroller.cpp | endif
if &buftype ==# 'terminal'
  silent file Component/simplebatchcontroller.cpp
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
9
normal! zo
11
normal! zo
19
normal! zo
54
normal! zo
92
normal! zo
let s:l = 68 - ((54 * winheight(0) + 48) / 97)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 68
normal! 050|
wincmd w
argglobal
if bufexists(fnamemodify("main.cpp", ":p")) | buffer main.cpp | else | edit main.cpp | endif
if &buftype ==# 'terminal'
  silent file main.cpp
endif
balt include/minidnn/Utils/Random.h
setlocal fdm=expr
setlocal fde=v:lua.require'lazyvim.util'.ui.foldexpr()
setlocal fmr={{{,}}}
setlocal fdi=#
setlocal fdl=99
setlocal fml=1
setlocal fdn=20
setlocal fen
14
normal! zo
24
normal! zo
let s:l = 41 - ((40 * winheight(0) + 48) / 97)
if s:l < 1 | let s:l = 1 | endif
keepjumps exe s:l
normal! zt
keepjumps 41
normal! 05|
wincmd w
exe '1resize ' . ((&lines * 48 + 50) / 100)
exe 'vert 1resize ' . ((&columns * 104 + 157) / 314)
exe '2resize ' . ((&lines * 48 + 50) / 100)
exe 'vert 2resize ' . ((&columns * 104 + 157) / 314)
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
