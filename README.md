# Fake Identity Parser

A C++ application that scrapes fake identity data from fakenamegenerator.com and outputs it to JSON files.

## Features

- **Modular Architecture**: Clean separation of concerns with dedicated modules
- **Configurable**: JSON config file and environment variable support
- **Thread-safe Logging**: Centralized logging with configurable levels
- **Rate Limiting**: Built-in delays between requests
- **Retry Logic**: Automatic retry on HTTP failures
- **Signal Handling**: Graceful shutdown on SIGINT/SIGTERM
- **JSON Output**: All identities saved to timestamped JSON files

## Project Structure

```
fake_identity_parser-main/
├── CMakeLists.txt          # CMake build configuration
├── Makefile               # Traditional Makefile
├── config.json            # Configuration file
├── identity_parser.cpp    # Main application entry point
├── ConfigLoader.hpp/.cpp  # Configuration management
├── Logger.hpp/.cpp        # Thread-safe logging system
├── HttpClient.hpp/.cpp    # HTTP client and CURL wrapper
├── HtmlParser.hpp/.cpp    # HTML parsing and cleaning
├── JsonWriter.hpp/.cpp    # JSON file output
└── README.md              # This file
```

## Dependencies

- **CURL**: HTTP client library
- **pugixml**: XML/HTML parsing library
- **nlohmann/json**: JSON library
- **C++17**: Modern C++ standard

## Installation

### Prerequisites

Install required dependencies:

**Ubuntu/Debian:**
```bash
sudo apt update
sudo apt install build-essential libcurl4-openssl-dev libpugixml-dev nlohmann-json3-dev
```

**CentOS/RHEL:**
```bash
sudo yum groupinstall "Development Tools"
sudo yum install libcurl-devel pugixml-devel nlohmann-json-devel
```

### Building

**Using Make:**
```bash
make
```

**Using CMake:**
```bash
mkdir build && cd build
cmake ..
make
```

## Configuration

Create `config.json` in the project directory:

```json
{
    "description": "Configuration file for Fake Identity Parser",
    "version": "2.0",
    "settings": {
        "request_delay_sec": 2,
        "max_http_retries": 3,
        "log_file": "identity_parser.log",
        "log_level": "INFO"
    }
}
```

### Configuration Options

- `request_delay_sec` - Delay between requests in seconds (default: 2)
- `max_http_retries` - Maximum number of HTTP retry attempts (default: 3)
- `log_file` - Log file path (default: "identity_parser.log")
- `log_level` - Logging level: INFO, WARNING, ERROR, DEBUG (default: "INFO")

### Environment Variables

You can override config file settings with environment variables:
- `REQUEST_DELAY_SEC`
- `MAX_HTTP_RETRIES`
- `LOG_FILE`
- `LOG_LEVEL`

## Usage

### Basic Usage

```bash
./fake_identity_parser
```

### Command Line Options

```bash
./fake_identity_parser [options]

Options:
  --gender [male|female|random]     Gender of the identity (default: male)
  --country [US|UK|CA|AU|DE|FR|IT|ES]  Country code (default: US)
  --nameSet [common|arabic|chinese|japanese|russian]  Name set (default: common)
  --amount N                        Number of identities to generate (default: 1, max: 100)
  --randomize                       Randomize gender, country, and name set
  --randomGender                    Randomize gender
  --randomCountry                   Randomize country
  --randomNameSet                   Randomize name set
  --help                            Show this help message
```

### Examples

Generate 5 female US identities:
```bash
./fake_identity_parser --amount 5 --gender female --country US
```

Generate 10 random identities:
```bash
./fake_identity_parser --randomize --amount 10
```

## Output

The application generates JSON files with timestamps containing all scraped identities:

```json
[
    {
        "name": "John Doe",
        "address": "123 Main St",
        "city": "New York",
        "state": "New York",
        "zip_code": "10001",
        "email": "john.doe@example.com",
        "phone": "+1-555-123-4567",
        "username": "johndoe123",
        "password": "securepass123",
        "latitude": "40.7128",
        "longitude": "-74.0060",
        "user_agent": "Mozilla/5.0...",
        "gender": "male",
        "country": "US",
        "name_set": "common"
    }
]
```

## Logging

The application provides comprehensive logging with configurable levels:
- **INFO**: General application flow
- **WARNING**: Non-critical issues
- **ERROR**: Critical errors
- **DEBUG**: Detailed debugging information

Logs are written to both console and the configured log file.

## Security Notice

This tool is for development and testing purposes only. Do not use in production environments without proper security review.

## License

This project is provided as-is for educational purposes.