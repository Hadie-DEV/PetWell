# Step 1: Use a Linux image that already has GCC installed
FROM gcc:12.2

# Step 2: Set the working directory inside the container
WORKDIR /app

# Step 3: Copy your project files into the container
COPY . .

# Step 4: Compile your C program
RUN gcc -o myproject myproject.c

# Step 5: Run your program when the container starts
CMD ["./myproject"]



