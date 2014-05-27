all	:
	@echo "Please specify OS to build for. Do"
	@echo "  make linux"
	@echo "or"
	@echo "  make windows"

linux :
	@make -f Makefile_linux

windows :
	@make -f Makefile_windows
	
clean :
	-make -f Makefile_linux clean
	-make -f Makefile_windows clean
