CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
LDFLAGS = -lcurl -lpugixml

TARGET = fake_identity_parser
SOURCES = identity_parser.cpp ConfigLoader.cpp Logger.cpp HttpClient.cpp HtmlParser.cpp JsonWriter.cpp
OBJECTS = $(SOURCES:.cpp=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(TARGET)

install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

uninstall:
	rm -f /usr/local/bin/$(TARGET)

# Install dependencies (Ubuntu/Debian)
install-deps:
	sudo apt-get update
	sudo apt-get install -y \
		libmysqlclient-dev \
		libcurl4-openssl-dev \
		libpugixml-dev \
		nlohmann-json3-dev \
		build-essential

# Install dependencies (CentOS/RHEL/Fedora)
install-deps-rpm:
	sudo yum install -y \
		mysql-devel \
		libcurl-devel \
		pugixml-devel \
		gcc-c++

# Run with default settings
run: $(TARGET)
	./$(TARGET) --gender male --country US --nameSet common --amount 1

# Run with Docker database
run-docker: $(TARGET)
	docker-compose up -d mysql_db
	sleep 5  # Wait for database to start
	./$(TARGET) --gender male --country US --nameSet common --amount 1

# Stop Docker services
stop-docker:
	docker-compose down

# Note: Requires config.json for advanced configuration

.PHONY: all clean install-deps install-deps-rpm run run-docker stop-docker 