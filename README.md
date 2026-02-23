# Task Management System (C)

A simple task management system implemented in C using **linked lists** and **stacks**.

## Features
- Load tasks from a file (`tasks.txt`)
- Add / delete tasks
- Search tasks by ID or name
- View unperformed tasks
- Perform a task (moves it to performed list + pushes to stack)
- Undo last performed task
- View performed tasks
- Generate summary report (`Report.txt`)

## Data Structures Used
- **Linked List**: unperformed tasks, performed tasks
- **Stack (Linked List based)**: performed stack (for undo), undone stack

## Input File Format
The input file should be named `tasks.txt` and each line should follow this format:

`ID#TaskName#Date#Duration`

Example:
`101#Study DS#2026-02-23#2.5`

## How to Run
Compile:
```bash
gcc main.c -o task_manager