# Cluster Analysis

Cluster Analysis is a program that performs a simple clustering analysis using the single linkage method. It reads objects from a file, creates clusters based on the objects' coordinates, and merges the closest clusters until the desired number of clusters is achieved.

## Usage

To use the Cluster Analysis program, follow the steps below:

1. Compile the program: Run the following command in the terminal: gcc -o cluster_analysis cluster_analysis.c -lm
2. Run the program: Execute the following command in the terminal: ./cluster_analysis <filename> [N]

Replace `<filename>` with the name of the input file containing the objects' coordinates. Optionally, specify the desired number of clusters, N. If N is not provided, the program will use a default value.

3. View the results: The program will output the clusters formed based on the input data. Each cluster is displayed with its respective objects and coordinates.

## Input File Format

The input file should have the following format:
count=<number of objects>
id x y
id x y

- The first line specifies the number of objects in the file.
- Each subsequent line represents an object with its ID, X coordinate, and Y coordinate.

Note: The X and Y coordinates must be in the range of 0 to 1000.

## Examples

Here are some examples of how to use the Cluster Analysis program:

1. Analyze clusters in the input file "data.txt" and obtain 3 clusters: ./cluster_analysis data.txt 3
2. Analyze clusters in the input file "data.txt" using the default number of clusters: ./cluster_analysis data.txt

## Error Handling

The program performs error handling for various scenarios, including:

- Invalid command-line arguments.
- Inaccessible input file.
- Incorrect file format.
- Invalid or out-of-range coordinates.

## Contact

For any questions or feedback, please contact sasha.postelga@gmail.com.

