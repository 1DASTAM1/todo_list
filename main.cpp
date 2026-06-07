#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <clocale>
#include <cstdlib>
#include <vector>
#include <algorithm>

std::string filename = "list_tasks.txt";

struct Task {
    long long id; // Номер задачи
    std::string name; // Название задачи
    bool completed; // Состояние задачи (true - выполнена, false - не выполнена)
    std::string description; // Описание задачи
    int priority; // Приоритет задачи (от 1 до 10, где 1 - наивысший, а 10 - наименьший)
};

void clear_console() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}

#ifdef _WIN32 
    #include <windows.h>
#endif

void setRussianLanguage() {
    #ifdef _WIN32
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
    #else
        setlocale(LC_ALL, "Russian");
    #endif
}

bool file_exists() {
    bool exists = std::filesystem::exists(filename);
    if (!exists) std::cout << "Файл списка задач не найден\n";
    return exists;
}

bool confirmation(std::string action) {
    std::cout << "Вы уверены, что хотите " << action << "?\n";
    std::cout << "Введите YES, чтобы подтвердить\n";
    std::getline(std::cin, action);
    if (action == "YES") {
        std::cout << "Выполнено\n";
        return true;
    }
    else {
        std::cout << "Не выполнено\n";
        return false;
    }
}

std::string name_task(std::string operation, int len_action) {
    std::string name;
    for (int i = len_action; i < operation.length(); i++) {
        if ((operation[i] == '{') || (operation[i] == '}') || (operation[i] == ' ')) continue;
        name += operation[i];
    }
    return name;
}

void list_add(std::string name) {
    Task add_task;
    add_task.name = name;
    add_task.completed = false;

    std::cout << "Напишите описание к задаче(в одной строке)\n";
    std::cout << "P.S.: В описании задачи не используйте символы '{' и '}', они будут удалены\n";
    while (true) {
        char symbol;
        std::cin.get(symbol);
        if (symbol == '\n') break;
        if ((symbol == '{') || (symbol == '}')) continue;
        add_task.description += symbol;
    }
    if (add_task.description.empty())
        add_task.description = "Описание отсутствует";
    std::cout << "\n";

    std::cout << "Дайте приоритет задаче (от 1 до 10, где 1 - наивысший, а 10 - наименьший)\n";
    int priority;
    std::string priority_string;
    std::getline(std::cin, priority_string);
    try {
        priority = std::stoi(priority_string);
    }
    catch (...) {
        std::cout << "Неверно введён приоритет, приоритет высотвлен наименьшим";
        priority = 10;
    }
    if ((priority < 1) || (priority > 10)) {
        std::cout << "Неверно введён приоритет, он должен быть от 0 до 10, приоритет высотвлен наименьшим";
        priority = 10;
    }
    add_task.priority = priority;
    std::cout << "\n";
    
    long long max_id = 0;
    std::ifstream readFile(filename);
    std::string line;
    while (std::getline(readFile, line)) {
        if (line == "{") {
            std::getline(readFile, line);
            long long id = std::stoll(line);
            if (id > max_id)
                max_id = id;
        }
    }
    readFile.close();
    add_task.id = max_id + 1;
    
    std::ofstream file(filename, std::ios::app);
    file << "{\n";
    file << add_task.id << "\n";
    file << add_task.name << "\n";
    file << add_task.completed << "\n";
    file << add_task.description << "\n";
    file << add_task.priority << "\n";
    file << "}\n";
    file.close();
}

void list_check() {
    std::ifstream file(filename);

    std::string line;
    bool empty = true;
    while (std::getline(file, line)) {
        if (line == "{") {
            empty = false;

            std::getline(file, line);
            std::cout << "ID задачи: "<< line << "\n";

            std::getline(file, line);
            std::cout << "Название задачи: "<< line << "\n";

            std::getline(file, line);
            if (line == "1") 
                std::cout << "Состояние задачи: выполнена\n";
            else
                std::cout << "Состояние задачи: не выполнена\n";
            
            std::getline(file, line);
            std::cout << "Описание задачи: "<< line << "\n";

            std::getline(file, line);
            std::cout << "Приоритет задачи: "<< line << "\n\n";
        }
    }
    file.close();

    if (empty)
        std::cout << "Файл со списком задач пуст\n";
}

void set_completed(long long id, bool completed) {
    std::ifstream readFile(filename);
    std::ofstream file("temporary.txt");

    std::string line;
    bool empty = true;
    bool nonId = true;

    while (std::getline(readFile, line)) {
        if (line == "{") {
            empty = false;

            file << "{\n";

            std::getline(readFile, line);
            bool idFound = stoll(line) == id;
            file << line << "\n";

            std::getline(readFile, line);
            file << line << "\n";

            std::getline(readFile, line);
            if (idFound) {
                file << completed << "\n";
                nonId = false;
            }
            else
                file << line << "\n";
            
            std::getline(readFile, line);
            file << line << "\n";

            std::getline(readFile, line);
            file << line << "\n";

            file << "}\n";
        }
    }
    readFile.close();
    file.close();

    std::filesystem::remove(filename);
    std::filesystem::rename("temporary.txt", filename);

    if (empty)
        std::cout << "Файл со списком задач пуст\n";
    else if (nonId)
        std::cout << "Id не найдено\n";
    else {
        if (completed)
            std::cout << "Задача отмечена выполненной\n";
        else 
            std::cout << "Задача отмечена не выполненной\n";
    }
}

void list_delete(long long id) {
    std::ifstream readFile(filename);
    std::ofstream file("temporary.txt");

    std::string line;
    bool empty = true;
    bool nonId = true;

    while (std::getline(readFile, line)) {
        if (line == "{") {
            empty = false;

            std::getline(readFile, line);

            if (stoll(line) != id) {
                file << "{\n";

                file << line << "\n";

                std::getline(readFile, line);
                file << line << "\n";

                std::getline(readFile, line);
                file << line << "\n";

                std::getline(readFile, line);
                file << line << "\n";

                std::getline(readFile, line);
                file << line << "\n";

                file << "}\n";
            }
            else {
                nonId = false;
                for (int i = 0; i < 4; i++)
                    std::getline(readFile, line);
            }
        }
    }
    readFile.close();
    file.close();

    std::filesystem::remove(filename);
    std::filesystem::rename("temporary.txt", filename);

    if (empty)
        std::cout << "Файл со списком задач пуст\n";
    else if (nonId)
        std::cout << "Id не найдено\n";
    else
        std::cout << "Задача удалена\n";
}

void sort_priority() {
    std::ifstream file(filename);

    std::vector<Task> list_task;

    std::string line;
    bool empty = true;
    bool nonSort = true;

    while (std::getline(file, line)) {
        if (line == "{") {
            empty = false;
            Task task;

            std::getline(file, line);
            task.id = stoll(line);

            std::getline(file, line);
            task.name = line;

            std::getline(file, line);
            if (line == "1")
                task.completed = true;
            else 
                task.completed = false;

            std::getline(file, line);
            task.description = line;

            std::getline(file, line);
            task.priority = stoi(line);

            if (!task.completed) {
                list_task.push_back(task);
                nonSort = false;
            }
        }
    }
    file.close();

    if (empty)
        std::cout << "Файл со списком задач пуст\n";
    else if (nonSort)
        std::cout << "Файл со списком задач имеет лишь выполненные задачи\n";
    else {
        std::sort(list_task.begin(), list_task.end(), 
            [](const Task& task1, const Task& task2) {
                return task1.priority > task2.priority;
            });
        
        while (!list_task.empty()) {
            Task task = list_task.back();
            list_task.pop_back();

            std::cout << "ID задачи: "<< task.id << "\n";

            std::cout << "Название задачи: "<< task.name << "\n";

            if (task.completed) 
                std::cout << "Состояние задачи: выполнена\n";
            else
                std::cout << "Состояние задачи: не выполнена\n";


            std::cout << "Описание задачи: "<< task.description << "\n";

            std::cout << "Приоритет задачи: "<< task.priority << "\n\n";
        }
    }
}

void set_priority(long long id, int priority) {
    std::ifstream readFile(filename);
    std::ofstream file("temporary.txt");

    std::string line;
    bool empty = true;
    bool nonId = true;

    while (std::getline(readFile, line)) {
        if (line == "{") {
            empty = false;

            file << "{\n";

            std::getline(readFile, line);
            bool idFound = id == stoll(line);

            file << line << "\n";

            std::getline(readFile, line);
            file << line << "\n";

            std::getline(readFile, line);
            file << line << "\n";
            
            std::getline(readFile, line);
            file << line << "\n";

            std::getline(readFile, line);
            if (idFound) {
                file << priority << "\n";
                nonId = false;
            }
            else
                file << line << "\n";

            file << "}\n";
        }
    }
    readFile.close();
    file.close();

    std::filesystem::remove(filename);
    std::filesystem::rename("temporary.txt", filename);

    if (empty)
        std::cout << "Файл со списком задач пуст\n";
    else if (nonId)
        std::cout << "Id не найдено\n";
    else {
        std::cout << "Приоритет задачи изменён\n";
    }
}
#include <windows.h>
int main() {
    setRussianLanguage();

    while (true) {
        std::cout << "=========================================\n";
        std::cout << "              Список задач\n";
        std::cout << "=========================================\n\n";
        std::cout << "Список команд:\n";
        std::cout << "1)   create_list - создать файл со списком задач\n";
        std::cout << "2)   delete_list - удалить файл со списком задач\n";
        std::cout << "3)   add <название задачи> - добавить задачу в список\n";
        std::cout << "4)   delete <номер задачи> - удалить задачу из списка\n";
        std::cout << "5)   set_priority <номер задачи> - изменить приоритет задачи\n";
        std::cout << "6)   list - вывести весь список задач\n";
        std::cout << "7)   done <номер задачи> - пометить задачу выполненой\n";
        std::cout << "8)   undone <номер задачи> - пометить задачу не выполненой\n";
        std::cout << "9)   priority - показать все невыполненные задачи от наивысшего приоритета до наименьшего\n";
        std::cout << "10)  clear - очистить список задач\n";
        std::cout << "11)  exit - выход из приложения\n\n";
        std::cout << "P.S.: Вписывайте именно команду, а не её номер; <> писать не надо; в названии задачи не используйте символы '{' и '}, они будут удалены'\n\n";
        std::cout << "Ввод команд: ";

        std::string operation;
        std::getline(std::cin, operation);
        clear_console();

        if (operation == "create_list") {
            if (file_exists()) {
                std::cout << "Файл со списоком задач уже создан, повтоное создание файла, удалит содержимое предыдущего\n";
                if (confirmation("создать файл повторно")) {
                    std::ofstream file(filename);
                    file.close();
                    std::cout << "Файл со списком задач создан\n"; 
                }
            }
            else {
                std::ofstream file(filename);
                file.close();
                std::cout << "Файл со списком задач создан\n";
            }
        }

        else if (operation == "delete_list") {
            if (file_exists()) {
                if (confirmation("удалить файл со списком задач")) {
                    std::filesystem::remove(filename);
                }
            }
        }

        else if (operation.find("add") == 0) {
            if (file_exists()) {
                std::string name = name_task(operation, 3); // 3 - длина "add"
                if (!name.empty())
                    list_add(name);
                else
                    std::cout << "Ошибка, название файла состоит из '{' и '}' либо пустое\n";
            }
        }

        else if (operation.find("delete") == 0) {
            if (file_exists()) {
                std::string id_string = name_task(operation, 6); // 6 - длина "delete"
                try {
                    long long id = std::stoll(id_string);
                    if (confirmation("удалить задачу")) {
                        list_delete(id);
                    }
                }
                catch (...) {
                    std::cout << "Неверно введён id или неуказан\n";
                }
            }
        }

        else if (operation.find("set_priority") == 0) {
            if (file_exists()) {
                long long id;
                bool error = false;

                std::string id_string = name_task(operation, 12); // 12 - длина "set_priority"
                try {
                    id = std::stoll(id_string);
                }
                catch (...) {
                    std::cout << "Неверно введён id или неуказан\n";
                    error = true;
                }

                if (!error) {
                    std::cout << "Дайте приоритет задаче (от 1 до 10, где 1 - наивысший, а 10 - наименьший)\n";

                    int priority;
                    std::string priority_string;

                    std::getline(std::cin, priority_string);
                    try {
                        priority = std::stoi(priority_string);
                    }
                    catch (...) {
                        std::cout << "Неверно введён приоритет\n";
                    }
                    if ((priority < 1) || (priority > 10)) {
                        std::cout << "Неверно введён приоритет, он должен быть от 0 до 10\n";
                    } else {
                        set_priority(id, priority);
                    }
                }
            }
        }

        else if (operation == "list") {
            if (file_exists()) {
                list_check();
            }
        }

        else if (operation.find("done") == 0) {
            if (file_exists()) {
                std::string id_string = name_task(operation, 4); // 4 - длина "done"
                try {
                    long long id = std::stoll(id_string);
                    set_completed(id, true);
                }
                catch (...) {
                    std::cout << "Неверно введён id или неуказан\n";
                }
            }
        }

        else if (operation.find("undone") == 0) {
            if (file_exists()) {
                std::string id_string = name_task(operation, 6); // 6 - длина "undone"
                try {
                    long long id = std::stoll(id_string);
                    set_completed(id, false);
                }
                catch (...) {
                    std::cout << "Неверно введён id или неуказан\n";
                }
            }
        }

        else if (operation == "priority") {
            if (file_exists()) {
                sort_priority();
            }
        }

        else if (operation == "clear") {
            if (file_exists()) {
                if (confirmation("очистить список задач")) {
                    std::ofstream file(filename);
                    file.close();
                    std::cout << "Файл со списком задач очищен\n"; 
                }
            }
        }

        else if (operation == "exit") {
            std::cout << "Приложение закрыто\n"; 
            break;
        }

        else {
            std::cout << "Ошибка, неверно введена команда\n"; 
        }

        std::cout << "Нажмите Enter, чтобы продолжить\n"; 
        std::cin.get();
        clear_console();
    }
    
    std::cout << "Нажмите Enter, чтобы продолжить\n"; 
    std::cin.get();
    return 0;
}
