import pandas as pd
import numpy as np


def normalize_array(array):
    normalized_array = (array - array.min()) / (array.max() - array.min())
    return normalized_array


# Load the spreadsheet
xlsx_path = './wenhao.xlsx'
df = pd.read_excel(xlsx_path)

# Show the dataframe
# print(df)

rotation_gain = 4.0
translation_gain = 3.0

rx_column = np.array(df['RX'].values[1:]).reshape(-1, 1) * rotation_gain
ry_column = np.array(df['RY'].values[1:]).reshape(-1, 1) * rotation_gain
rz_column = np.array(df['RZ'].values[1:]).reshape(-1, 1) * rotation_gain
tx_column = np.array(df['TX'].values[1:]).reshape(-1, 1)
ty_column = np.array(df['TY'].values[1:]).reshape(-1, 1)
tz_column = np.array(df['TZ'].values[1:]).reshape(-1, 1)

normalized_tx_column = normalize_array(tx_column) * translation_gain
normalized_ty_column = normalize_array(ty_column) * translation_gain
normalized_tz_column = normalize_array(tz_column) * translation_gain

data = np.hstack((normalized_tx_column, normalized_ty_column,
                 normalized_tz_column, rx_column, ry_column, rz_column))

print("data", data.shape)
# avg_tx_column = average_array(normalized_tx_column)
# print(avg_tx_column)
# print(normalized_ty_column)
# print(normalized_tz_column)

n = 50

num_chunks = data.shape[0] // n

# Trim the array to a size that's a multiple of 30
trimmed_array = data[:num_chunks * n]

# Reshape the array to have chunks of 30 rows
reshaped_array = trimmed_array.reshape(-1, n, data.shape[1])

# Take the mean across the rows of each chunk
averaged_array = reshaped_array.mean(axis=1)


print("averaged", averaged_array.shape)

# df = pd.DataFrame(averaged_array)

# # Specify your desired CSV file path
# csv_file_path = 'averaged_array.csv'

# # Save the DataFrame to a CSV file
# df.to_csv(csv_file_path, index=False)

# print(f"Averaged array saved to '{csv_file_path}'")
list_of_tuples = [tuple(row) for row in averaged_array]

# Format each number in the tuple to two decimal places and enclose in curly braces
formatted_string_with_precision = ",".join(
    "{" + ", ".join(f"{num:.2f}" for num in tup) + "}" for tup in list_of_tuples)

# Define the path for the new file
output_file_path_precise = 'vivon_4_3.txt'  # Change this path as needed

# Write the formatted string to the file, with two decimal precision for each number
with open(output_file_path_precise, 'w') as file:
    file.write(formatted_string_with_precision)

print(f"File saved to {output_file_path_precise}")
