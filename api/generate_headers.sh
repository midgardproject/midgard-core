
#valac --pkg MidgardCore --includedir=./core --vapidir=./core --library=Midgard -H public_headers/midgard_config.h vala/midgard_config.vala

valac --debug --pkg MidgardCore  --vapidir=/home/pp/git/piotras/midgard-core/api/core -H public_headers/midgard_config.h -C vala/midgard_config.vala
gcc --shared `pkg-config --cflags --libs glib-2.0` vala/midgard_config.c core/midgard_core_config.c

