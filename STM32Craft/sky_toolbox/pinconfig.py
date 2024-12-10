import os

def parse_ioc_file(ioc_file_path):
    """Parses the .ioc file and extracts pin configuration."""
    pin_configurations = []
    with open(ioc_file_path, 'r') as file:
        lines = file.readlines()
        in_pin_section = False

        for line in lines:
            line = line.strip()
            # Check for the PinConfiguration section
            if line == "[PinConfiguration]":
                in_pin_section = True
                continue

            if in_pin_section:
                # Stop reading once the section ends
                if line.startswith("["):
                    break
                if line:  # Non-empty lines in the section
                    pin_configurations.append(line)

    return pin_configurations

def generate_markdown(pin_configurations, output_file):
    """Generates a markdown file summarizing the pin configurations."""
    with open(output_file, 'w') as md_file:
        md_file.write("# STM32 Pin Configuration Summary\n\n")
        md_file.write("| Pin Name | Mode | Additional Settings |\n")
        md_file.write("|----------|------|---------------------|\n")
        
        for config in pin_configurations:
            # Parse each configuration line into a table row
            parts = config.split("=")
            pin_name = parts[0]
            details = parts[1].split(",")  # Example: Mode,Speed,Pulldown
            mode = details[0] if len(details) > 0 else "N/A"
            additional = ", ".join(details[1:]) if len(details) > 1 else "N/A"

            md_file.write(f"| {pin_name} | {mode} | {additional} |\n")
    
    print(f"Markdown summary generated: {output_file}")

def main():
    # Prompt user for the .ioc file path
    ioc_file_path = input("Enter the path to your STM32 .ioc file: ").strip()
    
    if not os.path.exists(ioc_file_path):
        print(f"Error: File {ioc_file_path} does not exist.")
        return

    # Define output directory and file
    output_dir = os.path.join("documentation", "pin_config")
    os.makedirs(output_dir, exist_ok=True)  # Create directories if they don't exist
    output_file = os.path.join(output_dir, "PinConfigurationSummary.md")

    # Parse and generate the markdown file
    pin_configurations = parse_ioc_file(ioc_file_path)
    if not pin_configurations:
        print("No pin configurations found in the .ioc file.")
        return

    generate_markdown(pin_configurations, output_file)

if __name__ == "__main__":
    main()
