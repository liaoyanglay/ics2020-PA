include nemu/Makefile.git

default:
	@echo "Please run 'make' under any subprojects to compile."

clean:
	-$(MAKE) -s -C nemu clean
	-$(MAKE) -s -C abstract-machine clean-all
	-$(MAKE) -s -C nanos-lite clean
	-$(MAKE) -s -C navy-apps clean

submit:
	# git gc
	STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s https://course.cunok.cn/pa/scripts/submit.sh)"

info: 
	STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s https://course.cunok.cn/pa/scripts/info.sh)"

setup: 
	STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s https://course.cunok.cn/pa/scripts/setup.sh)"

password: 
	STUID=$(STUID) STUNAME=$(STUNAME) bash -c "$$(curl -s https://course.cunok.cn/pa/scripts/password.sh)"

.PHONY: default clean submit info setup password
