# Portview

![Language](https://img.shields.io/badge/C++-17-00599C?style=flat-square)
![License](https://img.shields.io/badge/License-MIT-green?style=flat-square)
![Purpose](https://img.shields.io/badge/Purpose-Education-blue?style=flat-square)

Display local listening ports and connection information.

School Purpose Only.

## Overview

Portview is a polished educational cybersecurity utility by LayerByte. It focuses on one practical defensive concept and keeps the implementation small enough for students to read, run, and understand.

Knowing which local ports are listening helps students connect processes, services, and network exposure.

## Highlights

- Shows local listening ports
- Focuses on observation rather than scanning
- Keeps output easy to copy

## Feature Set

- Clear command-line or local application workflow
- Beginner-readable validation and error handling
- Copy-friendly output for notes, screenshots, and reports
- Conservative behavior designed around local or authorized data
- No exploit code, malware behavior, credential theft, brute forcing, or destructive actions

## Supported Inputs

- Local machine network state
- Listening ports visible to the current user

## Requirements

- A C++17 compatible compiler
- CMake 3.16 or newer when building with CMake

## Installation

Clone the repository and open the project folder:

```bash
git clone https://github.com/LayerByte/portview.git
cd portview
```

Then prepare the project with the standard toolchain:

```bash
cmake -S . -B build
cmake --build build
```

## Usage

Start with the help command or the default run command:

```bash
.\\build\\portview --help
```

## Example Workflow

1. Open the project folder.
2. Run the help command.
3. Provide a small authorized sample input.
4. Review the report and compare it with the source code.

Example run:

```bash
.\\build\\portview --help
```

## Expected Output

A local port summary for authorized system review.

Output is intended to be readable in the terminal or application window and easy to copy into a school report or defensive analysis note.

## Safety Scope

- Use only on systems, files, domains, and data you own or have permission to inspect.
- Treat paths, hostnames, hashes, and log entries as potentially sensitive before sharing output.
- Prefer small sample files when learning how the tool works.
- Do not use the project for unauthorized scanning, exploitation, credential attacks, persistence, evasion, or destructive activity.

## Learning Goals

- Practice safe input validation and graceful error messages.
- Understand the defensive concept behind the tool.
- Learn how a focused security utility is organized in C++.
- Compare raw input with structured output.
- Build habits around permission, documentation, and responsible testing.

## Development Notes

Keep parsing defensive, check file and argument errors, and avoid platform-specific behavior unless it is clearly guarded.

Suggested checks before publishing changes:

```bash
# Run the help command.
# Test with a small non-sensitive sample.
# Confirm errors are clear when input is missing or invalid.
```

## Troubleshooting

- If the command is not found, confirm the required toolchain is installed and available in your PATH.
- If a file cannot be opened, check the path, permissions, and whether another program is locking it.
- If a network-focused check fails, verify the hostname, scheme, connection, and permission to test that endpoint.
- If output looks empty, retry with a smaller known-good sample input.

## Known Limitations

- Built for education and small authorized workflows, not enterprise monitoring.
- Results depend on operating system permissions, platform APIs, and sample quality.
- Some advanced features are intentionally omitted to keep the code approachable.
- Findings should be reviewed by a human before making security decisions.

## Disclaimer

This project is for defensive learning, school assignments, and authorized administration. It does not include malware, credential theft, brute-force attacks, exploitation, payload delivery, persistence, bypass functionality, or unauthorized access functionality.

## License

Released under the MIT License.
