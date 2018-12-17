CPPFLAGS=-I/opt/opencv/include
CXXFLAGS=-Wall -Wextra
LDFLAGS=-Wl,-R/opt/opencv/lib -L/opt/opencv/lib
LDLIBS=\
	-lopencv_core\
	-lopencv_imgproc\
	-lopencv_highgui

BIN=\
	line_detector\
	cursors\
	line_detecto_test\
	hough

.PHONY: all
all: $(BIN)


.PHONY: test
test:
	./line_detector data/log1/144-rgb.png
	./line_detector data/log1/157-rgb.png
	./line_detector data/log1/171-rgb.png

	./line_detector data/log2/179-rgb.png
	./line_detector data/log2/194-rgb.png
	./line_detector data/log2/338-rgb.png

	./line_detector data/log3/054-rgb.png
	./line_detector data/log3/076-rgb.png
	./line_detector data/log3/243-rgb.png

	./line_detector data/log4/04-rgb.png
	./line_detector data/log4/46-rgb.png
	./line_detector data/log4/23-rgb.png

# .PHONY: cursors
# cursors:
# 	./cursors data/log1/144-rgb.png
# 	./cursors data/log1/157-rgb.png
# 	./cursors data/log1/171-rgb.png

# 	./cursors data/log2/179-rgb.png
# 	./cursors data/log2/194-rgb.png
# 	./cursors data/log2/338-rgb.png

# 	./cursors data/log3/054-rgb.png
# 	./cursors data/log3/076-rgb.png
# 	./cursors data/log3/243-rgb.png

# 	./cursors data/log4/04-rgb.png
# 	./cursors data/log4/46-rgb.png
# 	./cursors data/log4/23-rgb.png

.PHONY: clean
clean:
	$(RM) *~ *.png

.PHONY: cleanall
cleanall: clean
	$(RM) $(BIN) *.o
