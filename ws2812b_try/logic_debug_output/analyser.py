#C:\Users\paran\Desktop\Wonder\STM32_Craft\ws2812b_try\logic_debug_output\digital.csv
import pandas as pd

def classify_pulse(duration):
    """Classify the pulse based on WS2812B timing."""
    ws2812_timing = {
        "T0H": (0.4e-6, 0.15e-6),  # 0.4 µs ± 150 ns
        "T1H": (0.8e-6, 0.15e-6),  # 0.8 µs ± 150 ns
        "T0L": (0.85e-6, 0.15e-6), # 0.85 µs ± 150 ns
        "T1L": (0.45e-6, 0.15e-6), # 0.45 µs ± 150 ns
        "RES": (50e-6, None)       # Reset, > 50 µs
    }
    for pulse, (expected, tolerance) in ws2812_timing.items():
        if tolerance is None:
            if duration > expected:  # For RES
                return pulse
        elif abs(duration - expected) <= tolerance:  # Match with tolerance
            return pulse
    return "UNKNOWN"

def analyze_csv(file_path):
    """Analyze the CSV file and predict LED colors."""
    # Load the data
    data = pd.read_csv(file_path)

    # Calculate durations between transitions
    data['Duration [s]'] = data['Time [s]'].diff()

    # Classify pulses
    data['Pulse Type'] = data['Duration [s]'].apply(classify_pulse)

    # Extract binary data
    binary_data = []
    for _, row in data.iterrows():
        if row['Pulse Type'] == 'T0H':
            binary_data.append(0)
        elif row['Pulse Type'] == 'T1H':
            binary_data.append(1)

    # Group binary data into 24-bit segments (8 bits per channel: G, R, B)
    rgb_data = []
    for i in range(0, len(binary_data), 24):  # 24 bits per LED
        segment = binary_data[i:i+24]
        if len(segment) == 24:  # Ensure it's a complete RGB segment
            green = int("".join(map(str, segment[0:8])), 2)  # First 8 bits
            red = int("".join(map(str, segment[8:16])), 2)   # Next 8 bits
            blue = int("".join(map(str, segment[16:24])), 2) # Last 8 bits
            rgb_data.append((green, red, blue))

    # Output predictions
    for idx, (green, red, blue) in enumerate(rgb_data):
        print(f"LED {idx + 1}: Green={green}, Red={red}, Blue={blue}")

if __name__ == "__main__":
    # Path to the CSV file
    csv_file_path = r"C:\Users\paran\Desktop\Wonder\STM32_Craft\ws2812b_try\logic_debug_output\digital.csv"

    # Analyze the CSV file
    analyze_csv(csv_file_path)
