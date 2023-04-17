buffer: bufferManager.cpp queryProcessor.cpp
	g++ -Wall bufferManager.cpp queryProcessor.cpp -o q

data: databaseUploader.cpp
	g++ -Wall databaseUploader.cpp -o d