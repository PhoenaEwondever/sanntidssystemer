CC = ./buildroot/output/staging/usr/bin/avr32-linux-gcc 
FLAGS := -pthread
SRCS := ${wildcard *.c} ${wildcard **/*.c}

all:  ${SRCS}
	${CC} ${FLAGS} $^ -o main
	cp main /export/nfs/home/avr32/
	@./miniproject-server
	
sources: ${SRCS}
	@echo $^
	

NGW100: 
	mate-terminal -e "minicom -D /dev/ttyS0"
	
clean: 
	@echo "Cleaning.."
	@rm main
	@rm /export/nfs/home/avr32/main
