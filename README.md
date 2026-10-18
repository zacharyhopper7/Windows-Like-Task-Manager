# Exercise: Windows-Like Task Manager (Qt & C++)



## Introduction

This exercise challenges you to build a simplified "Windows-like Task Manager" application using Qt and C++. The core functionality will involve displaying a list of currently running system processes, allowing users to select a process, and then terminate it. To enhance user interaction, process termination will be achievable both through a dedicated button click and by implementing intuitive drag-and-drop functionality from the process list onto the termination button.

This project will provide practical experience with several key Qt concepts: `QProcess` for interacting with external system commands (to list and kill processes), Qt's Model/View framework (`QTableView` and `QStandardItemModel`) for displaying structured data, the signal and slot mechanism for event-driven programming, and Qt's event handling system for implementing custom drag-and-drop behavior. The goal is to create a functional desktop utility that demonstrates robust process management and user interaction.

----

## **Working with Dev Container**

To complete this assignment in a reliable and fully configured environment, please refer to the instructions in the file: **`README-devcontainer-docker.md`**. This guide walks you through opening the assignment in **Visual Studio Code** using a **Dev Container**, which automatically installs all necessary Python libraries and tools. Following that setup ensures that the notebook runs smoothly without manual configuration or missing dependencies. Make sure to open the **main assignment folder** in VS Code and follow the steps outlined in the Dev Container guide before starting the notebook.

----

## Starter Files & Setup

#### Prerequisites

- **Qt Framework (Version 6 recommended):** Widgets module is essential.
- **CMake Build System:** Version 3.16 or higher.
- **C++ Compiler:** A modern compiler supporting C++17 (e.g., GCC, Clang, MSVC).
- **Qt Creator IDE (Recommended):** Or another C++ IDE/editor with good CMake and Qt support (e.g., VS Code with C++ and CMake extensions).
- **Basic Understanding:** C++ fundamentals, Qt's `QObject` model, basic widget usage, and an understanding of system processes (PIDs, process names).

#### Starter Files

You will build this project from scratch.

1. Create a new project directory named `TaskManager`.
2. Inside `TaskManager`, you will create the necessary subdirectories and files as described below.

## Requirements

Implement a Task Manager application with the following features and structure, ensuring clean code and proper Qt practices.

### 1. Project Setup (`CMakeLists.txt`)

- Create a `CMakeLists.txt` file in the root of your `TaskManager` directory.
- Configure CMake to:
  - Specify a minimum required CMake version (e.g., 3.16).
  - Define the project name (e.g., `TaskManager`) and language (CXX).
  - Set the C++ standard (e.g., C++17).
  - Enable Qt's automatic tools: `CMAKE_AUTOMOC ON`, `CMAKE_AUTOUIC ON`.
  - Specify the search path for `.ui` files if they are in a subdirectory (e.g., `Forms/`).
  - Use `find_package(Qt6 REQUIRED COMPONENTS Widgets)` (or `Qt5` if using Qt5) to locate the Qt Widgets module.
  - Define an executable target (e.g., `TaskManager`) using `qt_add_executable` (or `add_executable`), listing all necessary source files (`.cpp`), header files (`.h` containing `Q_OBJECT`), and UI definition files (`.ui`).
  - Specify include directories using `target_include_directories`.
  - Link against the required Qt modules (e.g., `Qt6::Widgets`) using `target_link_libraries`.

### 2. UI Design (`Forms/taskmanager.ui`)

- Create a subdirectory `Forms` within `TaskManager`.
- Using Qt Designer, create a new "Widget" form named `taskmanager.ui` and save it in the `Forms` directory.
- Set the main widget's `objectName` to `TaskManager` (matching your class name) and `windowTitle` to "Task Manager".
- Apply a **`QVBoxLayout`** as the top-level layout for the `TaskManager` widget.
- Add the following widgets to the layout, in order from top to bottom:
  1. A **`QTableView`** named `processTable`.
     - Configure its `selectionMode` to `SingleSelection`.
     - Configure its `selectionBehavior` to `SelectRows`.
     - Set `editTriggers` to `NoEditTriggers` (read-only).
     - Set `dragEnabled` to `true`.
  2. A **`QPushButton`** named `endProcessButton` with the display text "End Process".
     - Set its `acceptDrops` property to `true`.
  3. A **`QLabel`** named `dragInstruction` with informative text like "Drag a process from the table onto the 'End Process' button to terminate it."
     - Set its `alignment` to `AlignCenter`.
- Save the `.ui` file. Use Qt Designer's "Preview" (Ctrl+R or Cmd+R) to check the layout.

### 3. Main Widget Class (`include/taskmanager.h` and `src/taskmanager.cpp`)

- Create an `include` directory and a `src` directory within `TaskManager`.

- **`include/taskmanager.h`**:
  
  - Define a class `TaskManager` that inherits from `QWidget`.
  - Include the `Q_OBJECT` macro.
  - Declare a private pointer `Ui::TaskManager *ui;` (forward declare `namespace Ui { class TaskManager; }`).
  - Declare private member variables:
    - `QProcess *processLister;` (for fetching process lists).
    - `QStandardItemModel *model;` (for the `QTableView`).
  - Declare a constructor: `explicit TaskManager(QWidget *parent = nullptr);`.
  - Declare a destructor: `~TaskManager();`.
  - Declare protected overridden event handlers for drag-and-drop:
    - `void dragEnterEvent(QDragEnterEvent *event) override;`
    - `void dropEvent(QDropEvent *event) override;`
  - Declare private `slots`:
    - `void loadProcesses();` (to trigger fetching/re-fetching the process list).
    - `void endProcess();` (connected to the button click).
    - `void updateProcessList();` (connected to `processLister->readyReadStandardOutput()`).
  - Declare private helper methods:
    - `void setupModel();` (to initialize table headers and model settings).
    - `void setupConnections();` (to connect signals and slots).
    - `bool confirmProcessKill(const QString &processId, const QString &processName);` (to show a confirmation dialog).
    - `void terminateProcessById(const QString &processId);` (to execute the kill command).

- **`src/taskmanager.cpp`**:
  
  - Include `taskmanager.h` and `ui_taskmanager.h` (which will be generated by `uic`).
  - **Constructor:**
    - Initialize `ui` with `new Ui::TaskManager`.
    - Call `ui->setupUi(this)`.
    - Initialize `processLister` and `model` (parented to `this`).
    - Set `this->setAcceptDrops(true);` on the main `TaskManager` widget to allow it to initially receive drop events.
    - Call `setupModel()`, `setupConnections()`.
    - Make an initial call to `loadProcesses()`.
    - Create a `QTimer`, connect its `timeout()` signal to the `loadProcesses()` slot, and start it (e.g., every 5000ms) for periodic updates.
  - **Destructor:**
    - Ensure `processLister` is stopped if running.
    - Delete `ui`. (Other `QObject` children are auto-deleted).
  - **`setupModel()`:**
    - Define column headers (e.g., "PID", "Process Name", "CPU %", "Memory"). Use platform-specific headers if desired (`#ifdef Q_OS_WIN` etc.).
    - Set the headers on the `model`.
    - Set the `model` on `ui->processTable`.
    - Configure `ui->processTable` properties as defined in the UI requirements (selection mode, behavior, no edit triggers, drag enabled, etc.).
  - **`setupConnections()`:**
    - Connect `ui->endProcessButton->clicked()` to `this->endProcess()`.
    - Connect `processLister->readyReadStandardOutput()` to `this->updateProcessList()`.
    - Connect `processLister->finished(int, QProcess::ExitStatus)` to a lambda or slot to handle process completion/errors from the lister.
    - Connect `processLister->errorOccurred(QProcess::ProcessError)` for `QProcess` specific errors.
  - **`loadProcesses()`:**
    - Check if `processLister` is already running. If so, return to avoid overlapping calls.
    - Clear the existing rows from the `model` (`model->removeRows(0, model->rowCount());`).
    - Use `processLister->start()` with the appropriate system command to list processes:
      - Windows: `tasklist /FO CSV /NH` (No Header CSV format).
      - Unix-like (Linux/macOS): `ps aux` (or a more targeted `ps -e -o pid,pcpu,pmem,comm`).
    - Optionally, call `processLister->waitForStarted()` with a short timeout and handle failure.
  - **`updateProcessList()`:**
    - Read all data from `processLister->readAllStandardOutput()`.
    - Parse the output string (line by line, splitting by delimiter - comma for CSV, whitespace for `ps aux`).
    - For each process, create `QStandardItem`s for each piece of data.
    - **Crucially:** For the PID item, store the PID string additionally using `pidItem->setData(pidString, Qt::UserRole);`. This allows reliable PID retrieval later.
    - Append the list of `QStandardItem*` as a new row to the `model`.
  - **`dragEnterEvent(QDragEnterEvent *event)`:**
    - Check if `event->mimeData()->hasText()` (or a custom MIME type if you set one during drag start).
    - Check if `event->source() == ui->processTable` to ensure the drag originated from the table.
    - If conditions met, call `event->acceptProposedAction()`. Otherwise, `event->ignore()`.
  - **`dropEvent(QDropEvent *event)`:**
    - Check if `ui->endProcessButton->geometry().contains(event->pos())` (i.e., was dropped onto the button).
    - If so, extract the PID from `event->mimeData()->text()`. **Note:** The default text drag from `QTableView` might be just the display text of the first column. A more robust way would involve a custom model `mimeData()` function, but for this exercise, assume the text contains identifiable PID info or attempt to get the PID from the table's _current selection_ (though this can be tricky if selection changes during drag).
    - For simplicity in PID extraction from drop: Assume the `mimeData()->text()` is just the PID. (A better method is to implement `QAbstractItemModel::mimeData` to store the PID in a custom format or `UserRole`).
    - Call `confirmProcessKill()` and then `terminateProcessById()`.
    - Call `event->acceptProposedAction()` if handled.
  - **`endProcess()`:**
    - Get the currently selected row(s) from `ui->processTable->selectionModel()`.
    - If a row is selected, retrieve the PID item from that row (e.g., from column 1 for `ps aux`).
    - Get the PID string from `pidItem->data(Qt::UserRole).toString()`.
    - Get a process name (e.g., from column 0 for Windows or column 10 for `ps aux`) for the confirmation dialog using `data(Qt::DisplayRole)`.
    - Call `confirmProcessKill()` and then `terminateProcessById()`.
    - Show a warning `QMessageBox` if no process is selected.
  - **`confirmProcessKill(const QString &processId, const QString &processName)`:**
    - Use `QMessageBox::question()` to ask the user for confirmation.
    - Return `true` if "Yes" is clicked, `false` otherwise.
  - **`terminateProcessById(const QString &processId)`:**
    - Create a _new_ `QProcess` object (local to this function or a member if you need to track it).
    - Use platform-specific commands to terminate the process by PID:
      - Windows: `taskkill /PID <processId> /F` (force kill).
      - Unix-like: `kill <processId>` (sends SIGTERM by default). Use `kill -9 <processId>` for SIGKILL if needed.
    - Call `waitForFinished()` on the killer process with a short timeout.
    - Check `exitCode()` of the killer process and show a success/failure `QMessageBox` to the user.
    - Call `loadProcesses()` to refresh the list after attempting termination.

### 4. Entry Point (`src/main.cpp`)

- Include `taskmanager.h` and `<QApplication>`.
- In `main()`, create a `QApplication` instance.
- Create an instance of your `TaskManager` widget.
- Call `taskManagerWidget.show()`.
- Call `app.exec()` to start the event loop.

## Deliverables

1. **Source Folder** (`TaskManager/`) containing:
   - `CMakeLists.txt` correctly configured.
   - `Forms/taskmanager.ui`, `include/taskmanager.h`, `src/taskmanager.cpp`, `src/main.cpp` fully implemented.
2. **Build Instructions** in a brief `README.md`:
   - How to configure and build the app (e.g., `mkdir build && cd build && cmake .. && cmake --build .` or `make`).
   - How to run the app (e.g., `./TaskManager` from the build directory).
3. **Code Comments** explaining key logic:
   - Parsing `ps` or `tasklist` output.
   - Storing/retrieving PID using `Qt::UserRole`.
   - `dragEnterEvent` and `dropEvent` logic.
   - Signal-slot connections for process updates and termination.

_(Focus on functional process listing, button termination, drag-and-drop termination, user confirmation, and basic error/feedback messages. Robust parsing of all `ps aux` fields or advanced features are secondary.)_

## Conclusion

By completing this exercise, you will have built a functional Task Manager application that demonstrates key Qt Widgets concepts, including `QProcess` for system interaction, Model/View for data display, event handling for drag-and-drop, and the signal-slot mechanism for responsive UI updates. This project serves as a practical application of these concepts to create a useful desktop utility.
