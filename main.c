// Advanced Student Management System with Topper Features
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Constants
#define MAX_USERS 100
#define MAX_STUDENTS 100
#define MAX_SUBJECTS 6
#define MAX_TERMS 3

// Function prototypes
void clearScreen();
void pauseProgram();
void clearInputBuffer();
void displayBanner();
void displayMainMenu();
int login();
void signUp();
void generateOTP(char* otp);
void sendOTP(const char* otp);
int verifyOTP(const char* otp);
int studentExists(long int rollNumber, const char* fullName);
void manageDatabase();
void addStudentData();
void editStudentData();
void deleteStudentData();
void searchStudentResult();
void displayStudentMarkSheet(const char* fullName, long int rollNumber);
void listAllStudents();
void viewTopPerformersBySubject();
void viewTopPerformersByClass();
void backupData();
void restoreData();
void logActivity(const char* activity);
void loadUsers();
void loadStudents();
void saveUsers();
void saveStudents();
void hashPassword(const char* password, char* hashedPassword);
const char* getGrade(float marks);
float calculateGPA(float marks[], int count);
void printLine(char ch, int length);
void generatePersonalizedMessage(float GPA);

// Structs
typedef struct {
    char email[100];
    char username[50];
    char fullName[100];
    char password[65]; // For hashed password
    char subjects[MAX_SUBJECTS][30]; // Custom subjects
} User;

typedef struct {
    char fullName[100];
    long int rollNumber;
    int class;
    int termsEntered;
    float marks[MAX_TERMS][MAX_SUBJECTS];
    float GPA[MAX_TERMS];
} Student;

// Global Variables
User users[MAX_USERS];
int userCount = 0;
Student students[MAX_STUDENTS];
int studentCount = 0;
char currentUser[100];
char currentUserSubjects[MAX_SUBJECTS][30]; // Subjects for the current user
FILE* logFile = NULL;

// Function Implementations

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void pauseProgram() {
    printf("\nPress Enter to continue...");
    getchar();
}

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void printLine(char ch, int length) {
    for (int i = 0; i < length; i++) {
        printf("%c", ch);
    }
    printf("\n");
}

void displayBanner() {
    printLine('=', 50);
    printf("\n           TRITON COLLEGE, NEPAL          \n");
    printf("         STUDENT MANAGEMENT SYSTEM        \n");
    printLine('=', 50);
    printf("\n");
}

void displayMainMenu() {
    printf("\nMain Menu:\n");
    printLine('-', 50);
    printf("1. Manage Database\n");
    printf("2. Search Student Result\n");
    printf("3. List All Students\n");
    printf("4. View Top Performers by Subject\n");
    printf("5. View Top Performers by Class\n");
    printf("6. Backup Data\n");
    printf("7. Restore Data\n");
    printf("8. Logout\n");
    printf("9. Exit\n");
    printLine('-', 50);
    printf("\nEnter your choice: ");
}

int main() {
    // Open log file
    logFile = fopen("activity.log", "a");
    if (logFile == NULL) {
        printf("Error opening log file.\n");
        return 1;
    }

    loadUsers();
    loadStudents();

    int isLoggedIn = 0;
    while (!isLoggedIn) {
        clearScreen();
        displayBanner();
        printf("\n1. Login\n");
        printf("2. Sign Up\n");
        printf("3. Exit\n");
        printLine('-', 50);
        printf("\nEnter your choice: ");
        int choice;
        scanf("%d", &choice);
        clearInputBuffer();
        if (choice == 1) {
            isLoggedIn = login();
        } else if (choice == 2) {
            signUp();
        } else if (choice == 3) {
            fclose(logFile);
            exit(0);
        } else {
            printf("\nInvalid choice.\n");
            pauseProgram();
        }
    }

    int exitProgram = 0;
    while (!exitProgram) {
        clearScreen();
        displayBanner();
        printf("\nWelcome, %s!\n", currentUser);
        displayMainMenu();
        int choice;
        scanf("%d", &choice);
        clearInputBuffer();
        switch (choice) {
            case 1:
                manageDatabase();
                break;
            case 2:
                searchStudentResult();
                break;
            case 3:
                listAllStudents();
                break;
            case 4:
                viewTopPerformersBySubject();
                break;
            case 5:
                viewTopPerformersByClass();
                break;
            case 6:
                backupData();
                break;
            case 7:
                restoreData();
                break;
            case 8:
                isLoggedIn = 0;
                printf("\nLogged out successfully.\n");
                logActivity("User logged out");
                pauseProgram();
                fclose(logFile);
                return main(); // Restart the program
            case 9:
                exitProgram = 1;
                break;
            default:
                printf("\nInvalid choice.\n");
                pauseProgram();
                break;
        }
    }

    fclose(logFile);
    return 0;
}

int login() {
    clearScreen();
    displayBanner();
    printf("\nLOGIN:\n");
    printLine('-', 50);
    char identifier[100];
    char password[50];
    char hashedPassword[65];
    printf("Enter Username or Email ID: ");
    fgets(identifier, sizeof(identifier), stdin);
    identifier[strcspn(identifier, "\n")] = '\0';

    printf("Enter Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = '\0';

    hashPassword(password, hashedPassword);

    // Check credentials
    for (int i = 0; i < userCount; i++) {
        if ((strcmp(users[i].username, identifier) == 0 || strcmp(users[i].email, identifier) == 0) &&
            strcmp(users[i].password, hashedPassword) == 0) {

            // Generate and verify OTP
            char otp[7]; // Format: XXXXXX (6 digits)
            generateOTP(otp);
            sendOTP(otp);
            printf("\nAn OTP has been sent to 'otp.txt'.\n");
            if (verifyOTP(otp)) {
                strcpy(currentUser, users[i].fullName);
                // Copy user's subjects
                for (int j = 0; j < MAX_SUBJECTS; j++) {
                    strcpy(currentUserSubjects[j], users[i].subjects[j]);
                }
                printf("\nLogin successful! Welcome, %s.\n", currentUser);
                char activity[150];
                sprintf(activity, "User '%s' logged in", currentUser);
                logActivity(activity);
                pauseProgram();
                return 1;
            } else {
                printf("\nIncorrect OTP. Login failed.\n");
                logActivity("Failed OTP verification during login");
                pauseProgram();
                return 0;
            }
        }
    }
    printf("\nInvalid credentials.\n");
    logActivity("Failed login attempt with invalid credentials");
    pauseProgram();
    return 0;
}

void signUp() {
    clearScreen();
    displayBanner();
    printf("\nSIGN UP:\n");
    printLine('-', 50);

    if (userCount >= MAX_USERS) {
        printf("User limit reached. Cannot create more accounts.\n");
        pauseProgram();
        return;
    }

    User newUser;
    printf("Enter Full Name: ");
    fgets(newUser.fullName, sizeof(newUser.fullName), stdin);
    newUser.fullName[strcspn(newUser.fullName, "\n")] = '\0';

    printf("Enter Email ID: ");
    fgets(newUser.email, sizeof(newUser.email), stdin);
    newUser.email[strcspn(newUser.email, "\n")] = '\0';

    printf("Enter Username: ");
    fgets(newUser.username, sizeof(newUser.username), stdin);
    newUser.username[strcspn(newUser.username, "\n")] = '\0';

    printf("Enter Password: ");
    fgets(newUser.password, sizeof(newUser.password), stdin);
    newUser.password[strcspn(newUser.password, "\n")] = '\0';

    // Hash the password
    char hashedPassword[65];
    hashPassword(newUser.password, hashedPassword);
    strcpy(newUser.password, hashedPassword);

    // Set default subjects or customize
    char choice;
    printf("Do you want to customize subjects? (y/n): ");
    scanf("%c", &choice);
    clearInputBuffer();

    if (choice == 'y' || choice == 'Y') {
        printf("Enter the names of %d subjects:\n", MAX_SUBJECTS);
        for (int i = 0; i < MAX_SUBJECTS; i++) {
            printf("Subject %d: ", i + 1);
            fgets(newUser.subjects[i], sizeof(newUser.subjects[i]), stdin);
            newUser.subjects[i][strcspn(newUser.subjects[i], "\n")] = '\0';
        }
    } else {
        // Default subjects
        strcpy(newUser.subjects[0], "Mathematics");
        strcpy(newUser.subjects[1], "Science");
        strcpy(newUser.subjects[2], "English");
        strcpy(newUser.subjects[3], "Social Studies");
        strcpy(newUser.subjects[4], "Computer Science");
        strcpy(newUser.subjects[5], "Physical Education");
    }

    // Add to users array
    users[userCount++] = newUser;

    // Save users to file
    saveUsers();

    printf("\nAccount created successfully! Please log in to continue.\n");
    char activity[150];
    sprintf(activity, "New user '%s' signed up", newUser.username);
    logActivity(activity);
    pauseProgram();
}

void generateOTP(char* otp) {
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 6; i++) {
        otp[i] = '0' + rand() % 10;
    }
    otp[6] = '\0';
}

void sendOTP(const char* otp) {
    FILE* otpFile = fopen("otp.txt", "w");
    if (otpFile == NULL) {
        printf("Error creating OTP file.\n");
        return;
    }
    fprintf(otpFile, "Your OTP is: %s\n", otp);
    fclose(otpFile);
    logActivity("OTP generated and sent to 'otp.txt'");
}

int verifyOTP(const char* otp) {
    char enteredOTP[10];
    printf("Enter the OTP sent to 'otp.txt': ");
    fgets(enteredOTP, sizeof(enteredOTP), stdin);
    enteredOTP[strcspn(enteredOTP, "\n")] = '\0';
    return strcmp(otp, enteredOTP) == 0;
}

void logActivity(const char* activity) {
    if (logFile == NULL) {
        logFile = fopen("activity.log", "a");
        if (logFile == NULL) {
            printf("Error opening log file.\n");
            return;
        }
    }
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    fprintf(logFile, "[%02d-%02d-%04d %02d:%02d:%02d] %s\n",
            t->tm_mday, t->tm_mon + 1, t->tm_year + 1900,
            t->tm_hour, t->tm_min, t->tm_sec, activity);
    fflush(logFile);
}

int studentExists(long int rollNumber, const char* fullName) {
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNumber == rollNumber && strcmp(students[i].fullName, fullName) == 0) {
            return i;
        }
    }
    return -1;
}

void manageDatabase() {
    int choice = 0;
    while (choice != 4) {
        clearScreen();
        displayBanner();
        printf("\nManage Database:\n");
        printLine('-', 50);
        printf("1. Add Student Data\n");
        printf("2. Edit Student Data\n");
        printf("3. Delete Student Data\n");
        printf("4. Back to Main Menu\n");
        printLine('-', 50);
        printf("\nEnter your choice: ");
        scanf("%d", &choice);
        clearInputBuffer();

        switch (choice) {
            case 1:
                addStudentData();
                break;
            case 2:
                editStudentData();
                break;
            case 3:
                deleteStudentData();
                break;
            case 4:
                return;
            default:
                printf("Invalid choice.\n");
                pauseProgram();
                break;
        }
    }
}

void addStudentData() {
    clearScreen();
    displayBanner();
    printf("\nAdd Student Data:\n");
    printLine('-', 50);

    if (studentCount >= MAX_STUDENTS) {
        printf("Student limit reached. Cannot add more students.\n");
        pauseProgram();
        return;
    }

    Student newStudent;

    printf("Enter Full Name: ");
    fgets(newStudent.fullName, sizeof(newStudent.fullName), stdin);
    newStudent.fullName[strcspn(newStudent.fullName, "\n")] = '\0';

    printf("Enter Roll Number: ");
    char rollInput[20];
    fgets(rollInput, sizeof(rollInput), stdin);
    sscanf(rollInput, "%ld", &newStudent.rollNumber);

    printf("Enter Class: ");
    char classInput[10];
    fgets(classInput, sizeof(classInput), stdin);
    sscanf(classInput, "%d", &newStudent.class);

    // Check if student already exists
    if (studentExists(newStudent.rollNumber, newStudent.fullName) != -1) {
        printf("\nStudent already exists.\n");
        pauseProgram();
        return;
    }

    // Ask for number of terms
    printf("\nEnter number of terms you want to input marks for (1-%d): ", MAX_TERMS);
    char termInput[10];
    fgets(termInput, sizeof(termInput), stdin);
    sscanf(termInput, "%d", &newStudent.termsEntered);

    if (newStudent.termsEntered < 1 || newStudent.termsEntered > MAX_TERMS) {
        printf("\nInvalid number of terms.\n");
        pauseProgram();
        return;
    }

    // Input marks for the terms
    for (int term = 0; term < newStudent.termsEntered; term++) {
        printf("\nEnter marks for Term %d:\n", term + 1);
        printLine('-', 30);
        for (int i = 0; i < MAX_SUBJECTS; i++) {
            while (1) {
                printf("%s: ", currentUserSubjects[i]);
                char markInput[10];
                fgets(markInput, sizeof(markInput), stdin);
                if (sscanf(markInput, "%f", &newStudent.marks[term][i]) == 1 &&
                    newStudent.marks[term][i] >= 0 && newStudent.marks[term][i] <= 100) {
                    break;
                } else {
                    printf("Invalid input. Please enter a number between 0 and 100.\n");
                }
            }
        }
        newStudent.GPA[term] = calculateGPA(newStudent.marks[term], MAX_SUBJECTS);
    }

    // Add to students array
    students[studentCount++] = newStudent;

    // Save to students file
    saveStudents();

    printf("\nStudent data added successfully!\n");
    char activity[150];
    sprintf(activity, "User '%s' added student '%s'", currentUser, newStudent.fullName);
    logActivity(activity);
    pauseProgram();
}

void editStudentData() {
    clearScreen();
    displayBanner();
    printf("\nEdit Student Data:\n");
    printLine('-', 50);

    long int rollNumber;
    char name[100];

    printf("Enter Full Name of the Student: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Enter Roll Number: ");
    char rollInput[20];
    fgets(rollInput, sizeof(rollInput), stdin);
    sscanf(rollInput, "%ld", &rollNumber);

    int index = studentExists(rollNumber, name);
    if (index == -1) {
        printf("\nStudent not found.\n");
        pauseProgram();
        return;
    }

    Student* student = &students[index];

    // Ask if adding marks for a new term or editing existing term
    printf("\nDo you want to add marks for a new term or edit existing term?\n");
    printf("1. Add new term marks\n");
    printf("2. Edit existing term marks\n");
    printf("Enter your choice: ");
    int choice;
    scanf("%d", &choice);
    clearInputBuffer();

    if (choice == 1) {
        if (student->termsEntered >= MAX_TERMS) {
            printf("\nAll terms already have marks entered.\n");
            pauseProgram();
            return;
        }
        int term = student->termsEntered;
        printf("\nEnter marks for Term %d:\n", term + 1);
        printLine('-', 30);
        for (int i = 0; i < MAX_SUBJECTS; i++) {
            while (1) {
                printf("%s: ", currentUserSubjects[i]);
                char markInput[10];
                fgets(markInput, sizeof(markInput), stdin);
                if (sscanf(markInput, "%f", &student->marks[term][i]) == 1 &&
                    student->marks[term][i] >= 0 && student->marks[term][i] <= 100) {
                    break;
                } else {
                    printf("Invalid input. Please enter a number between 0 and 100.\n");
                }
            }
        }
        student->GPA[term] = calculateGPA(student->marks[term], MAX_SUBJECTS);
        student->termsEntered++;
    } else if (choice == 2) {
        printf("\nEnter which term's marks you want to edit (1-%d): ", student->termsEntered);
        int term;
        scanf("%d", &term);
        clearInputBuffer();
        term -= 1; // Adjust for array index

        if (term < 0 || term >= student->termsEntered) {
            printf("\nInvalid term number.\n");
            pauseProgram();
            return;
        }

        printf("\nEnter new marks for Term %d:\n", term + 1);
        printLine('-', 30);
        for (int i = 0; i < MAX_SUBJECTS; i++) {
            while (1) {
                printf("%s: ", currentUserSubjects[i]);
                char markInput[10];
                fgets(markInput, sizeof(markInput), stdin);
                if (sscanf(markInput, "%f", &student->marks[term][i]) == 1 &&
                    student->marks[term][i] >= 0 && student->marks[term][i] <= 100) {
                    break;
                } else {
                    printf("Invalid input. Please enter a number between 0 and 100.\n");
                }
            }
        }
        student->GPA[term] = calculateGPA(student->marks[term], MAX_SUBJECTS);
    } else {
        printf("\nInvalid choice.\n");
        pauseProgram();
        return;
    }

    // Save to students file
    saveStudents();

    printf("\nStudent data updated successfully!\n");
    char activity[150];
    sprintf(activity, "User '%s' edited data of student '%s'", currentUser, name);
    logActivity(activity);
    pauseProgram();
}

void deleteStudentData() {
    clearScreen();
    displayBanner();
    printf("\nDelete Student Data:\n");
    printLine('-', 50);

    long int rollNumber;
    char name[100];

    printf("Enter Full Name of the Student: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Enter Roll Number: ");
    char rollInput[20];
    fgets(rollInput, sizeof(rollInput), stdin);
    sscanf(rollInput, "%ld", &rollNumber);

    int index = studentExists(rollNumber, name);
    if (index == -1) {
        printf("\nStudent not found.\n");
        pauseProgram();
        return;
    }

    // Shift students to overwrite the deleted student
    for (int i = index; i < studentCount - 1; i++) {
        students[i] = students[i + 1];
    }
    studentCount--;

    // Save to students file
    saveStudents();

    printf("\nStudent data deleted successfully!\n");
    char activity[150];
    sprintf(activity, "User '%s' deleted data of student '%s'", currentUser, name);
    logActivity(activity);
    pauseProgram();
}

void searchStudentResult() {
    clearScreen();
    displayBanner();
    printf("\nSearch Student Result:\n");
    printLine('-', 50);

    long int rollNumber;
    char name[100];

    printf("Enter Full Name of the Student: ");
    fgets(name, sizeof(name), stdin);
    name[strcspn(name, "\n")] = '\0';

    printf("Enter Roll Number: ");
    char rollInput[20];
    fgets(rollInput, sizeof(rollInput), stdin);
    sscanf(rollInput, "%ld", &rollNumber);

    int index = -1;
    for (int i = 0; i < studentCount; i++) {
        if (students[i].rollNumber == rollNumber && strcmp(students[i].fullName, name) == 0) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        printf("\nStudent not found.\n");
        pauseProgram();
        return;
    }

    // Display marksheet
    displayStudentMarkSheet(students[index].fullName, students[index].rollNumber);
    pauseProgram();
}

void displayStudentMarkSheet(const char* fullName, long int rollNumber) {
    int index = studentExists(rollNumber, fullName);
    if (index == -1) {
        printf("\nStudent data not found.\n");
        return;
    }

    Student* student = &students[index];

    clearScreen();
    printf("\n");
    printLine('=', 50);
    printf("                 MARKSHEET                  \n");
    printLine('=', 50);
    printf("Student Name : %s\n", student->fullName);
    printf("Roll Number  : %ld\n", student->rollNumber);
    printf("Class        : %d\n", student->class);
    printLine('=', 50);

    // Display only terms with entered marks
    for (int term = 0; term < student->termsEntered; term++) {
        printf("\n---------- Term %d Results ----------\n", term + 1);
        printLine('-', 50);
        printf("%-25s%-10s%-10s%-10s\n", "Subject", "Marks", "Grade", "Topper");
        printLine('-', 50);
        for (int i = 0; i < MAX_SUBJECTS; i++) {
            const char* grade = getGrade(student->marks[term][i]);
            int isSubjectTopper = 1;
            float mark = student->marks[term][i];
            for (int j = 0; j < studentCount; j++) {
                if (students[j].marks[term][i] > mark) {
                    isSubjectTopper = 0;
                    break;
                }
            }
            printf("%-25s%-10.2f%-10s%-10s\n", currentUserSubjects[i], mark, grade, isSubjectTopper ? "Yes" : "No");
        }
        printf("\nGPA for Term %d: %.2f\n", term + 1, student->GPA[term]);

        // Check if the student is the class topper
        int isClassTopper = 1;
        for (int j = 0; j < studentCount; j++) {
            if (students[j].class == student->class && students[j].GPA[term] > student->GPA[term]) {
                isClassTopper = 0;
                break;
            }
        }
        printf("Class Topper: %s\n", isClassTopper ? "Yes" : "No");

        generatePersonalizedMessage(student->GPA[term]);
        printLine('=', 50);
    }

    char activity[150];
    sprintf(activity, "User '%s' viewed marksheet of student '%s'", currentUser, fullName);
    logActivity(activity);
}

void generatePersonalizedMessage(float GPA) {
    printf("\nPersonalized Message:\n");
    if (GPA >= 3.6) {
        printf("Outstanding performance! You're leading the way.\nKeep shining bright!\n");
    } else if (GPA >= 3.0) {
        printf("Great job! You're doing really well.\nStay focused and keep pushing.\n");
    } else if (GPA >= 2.0) {
        printf("Good effort! There's potential for improvement.\nLet's aim higher next time.\n");
    } else {
        printf("Don't lose heart. Reflect and strategize.\nYou've got this!\n");
    }
}

void listAllStudents() {
    clearScreen();
    displayBanner();
    printf("\nList of All Students:\n\n");
    printLine('-', 50);

    printf("%-5s%-30s%-15s%-10s\n", "S.N.", "Full Name", "Roll Number", "Class");
    printLine('-', 50);
    for (int i = 0; i < studentCount; i++) {
        printf("%-5d%-30s%-15ld%-10d\n", i + 1, students[i].fullName, students[i].rollNumber, students[i].class);
    }
    printLine('-', 50);

    pauseProgram();
}

void viewTopPerformersBySubject() {
    clearScreen();
    displayBanner();
    printf("\nView Top Performers by Subject:\n");
    printLine('-', 50);

    for (int i = 0; i < MAX_SUBJECTS; i++) {
        printf("%d. %s\n", i, currentUserSubjects[i]);
    }

    printf("\nEnter Subject Index (0-%d): ", MAX_SUBJECTS - 1);
    int subjectIndex;
    scanf("%d", &subjectIndex);
    clearInputBuffer();

    if (subjectIndex < 0 || subjectIndex >= MAX_SUBJECTS) {
        printf("\nInvalid subject index.\n");
        pauseProgram();
        return;
    }

    float highestMarks = -1;
    for (int i = 0; i < studentCount; i++) {
        for (int term = 0; term < students[i].termsEntered; term++) {
            if (students[i].marks[term][subjectIndex] > highestMarks) {
                highestMarks = students[i].marks[term][subjectIndex];
            }
        }
    }

    printf("\nTop Performers in %s:\n", currentUserSubjects[subjectIndex]);
    printLine('-', 50);
    printf("%-30s%-10s%-10s%-10s\n", "Full Name", "Class", "Roll No", "Marks");
    printLine('-', 50);

    for (int i = 0; i < studentCount; i++) {
        int isTopper = 0;
        for (int term = 0; term < students[i].termsEntered; term++) {
            if (students[i].marks[term][subjectIndex] == highestMarks) {
                isTopper = 1;
                break;
            }
        }
        if (isTopper) {
            printf("%-30s%-10d%-10ld%-10.2f\n", students[i].fullName, students[i].class, students[i].rollNumber, highestMarks);
        }
    }

    pauseProgram();
}

void viewTopPerformersByClass() {
    clearScreen();
    displayBanner();
    printf("\nView Top Performers by Class:\n");
    printLine('-', 50);

    printf("Enter Class: ");
    int classNumber;
    scanf("%d", &classNumber);
    clearInputBuffer();

    float highestGPA = -1;
    for (int i = 0; i < studentCount; i++) {
        if (students[i].class == classNumber) {
            for (int term = 0; term < students[i].termsEntered; term++) {
                if (students[i].GPA[term] > highestGPA) {
                    highestGPA = students[i].GPA[term];
                }
            }
        }
    }

    printf("\nTop Performers in Class %d:\n", classNumber);
    printLine('-', 50);
    printf("%-30s%-10s%-10s%-10s\n", "Full Name", "Class", "Roll No", "GPA");
    printLine('-', 50);

    for (int i = 0; i < studentCount; i++) {
        int isTopper = 0;
        if (students[i].class == classNumber) {
            for (int term = 0; term < students[i].termsEntered; term++) {
                if (students[i].GPA[term] == highestGPA) {
                    isTopper = 1;
                    break;
                }
            }
        }
        if (isTopper) {
            printf("%-30s%-10d%-10ld%-10.2f\n", students[i].fullName, students[i].class, students[i].rollNumber, highestGPA);
        }
    }

    pauseProgram();
}

void backupData() {
    clearScreen();
    displayBanner();
    printf("\nBacking up data...\n");

    // Backup users
    FILE* userFile = fopen("users_backup.txt", "w");
    if (userFile == NULL) {
        printf("Error backing up users.\n");
        return;
    }
    for (int i = 0; i < userCount; i++) {
        fprintf(userFile, "%s,%s,%s,%s\n",
                users[i].email,
                users[i].username,
                users[i].fullName,
                users[i].password);
        // Save subjects
        for (int j = 0; j < MAX_SUBJECTS; j++) {
            fprintf(userFile, "%s\n", users[i].subjects[j]);
        }
        fprintf(userFile, "END_SUBJECTS\n");
    }
    fclose(userFile);

    // Backup students
    FILE* studentFile = fopen("students_backup.txt", "wb");
    if (studentFile == NULL) {
        printf("Error backing up students.\n");
        return;
    }
    fwrite(&studentCount, sizeof(int), 1, studentFile);
    fwrite(students, sizeof(Student), studentCount, studentFile);
    fclose(studentFile);

    printf("Data backup completed successfully.\n");
    pauseProgram();
}

void restoreData() {
    clearScreen();
    displayBanner();
    printf("\nRestoring data...\n");

    // Restore users
    FILE* userFile = fopen("users_backup.txt", "r");
    if (userFile == NULL) {
        printf("No backup file found for users.\n");
    } else {
        userCount = 0;
        while (fscanf(userFile, "%99[^,],%49[^,],%99[^,],%64[^\n]\n",
                      users[userCount].email,
                      users[userCount].username,
                      users[userCount].fullName,
                      users[userCount].password) != EOF) {
            // Read subjects
            for (int j = 0; j < MAX_SUBJECTS; j++) {
                fgets(users[userCount].subjects[j], sizeof(users[userCount].subjects[j]), userFile);
                users[userCount].subjects[j][strcspn(users[userCount].subjects[j], "\n")] = '\0';
            }
            char endSubjects[20];
            fgets(endSubjects, sizeof(endSubjects), userFile); // Read END_SUBJECTS
            userCount++;
        }
        fclose(userFile);
    }

    // Restore students
    FILE* studentFile = fopen("students_backup.txt", "rb");
    if (studentFile == NULL) {
        printf("No backup file found for students.\n");
    } else {
        fread(&studentCount, sizeof(int), 1, studentFile);
        fread(students, sizeof(Student), studentCount, studentFile);
        fclose(studentFile);
    }

    printf("Data restoration completed successfully.\n");
    pauseProgram();
}

void loadUsers() {
    FILE* userFile = fopen("users.txt", "r");
    if (userFile == NULL) {
        return;
    }
    while (fscanf(userFile, "%99[^,],%49[^,],%99[^,],%64[^\n]\n",
                  users[userCount].email,
                  users[userCount].username,
                  users[userCount].fullName,
                  users[userCount].password) != EOF) {
        // Read subjects
        for (int j = 0; j < MAX_SUBJECTS; j++) {
            fgets(users[userCount].subjects[j], sizeof(users[userCount].subjects[j]), userFile);
            users[userCount].subjects[j][strcspn(users[userCount].subjects[j], "\n")] = '\0';
        }
        char endSubjects[20];
        fgets(endSubjects, sizeof(endSubjects), userFile); // Read END_SUBJECTS
        userCount++;
    }
    fclose(userFile);
}

void loadStudents() {
    FILE* studentFile = fopen("students.txt", "rb");
    if (studentFile == NULL) {
        return;
    }
    fread(&studentCount, sizeof(int), 1, studentFile);
    fread(students, sizeof(Student), studentCount, studentFile);
    fclose(studentFile);
}

void saveUsers() {
    FILE* userFile = fopen("users.txt", "w");
    if (userFile == NULL) {
        printf("Error saving users.\n");
        return;
    }
    for (int i = 0; i < userCount; i++) {
        fprintf(userFile, "%s,%s,%s,%s\n",
                users[i].email,
                users[i].username,
                users[i].fullName,
                users[i].password);
        // Save subjects
        for (int j = 0; j < MAX_SUBJECTS; j++) {
            fprintf(userFile, "%s\n", users[i].subjects[j]);
        }
        fprintf(userFile, "END_SUBJECTS\n");
    }
    fclose(userFile);
}

void saveStudents() {
    FILE* studentFile = fopen("students.txt", "wb");
    if (studentFile == NULL) {
        printf("Error saving students.\n");
        return;
    }
    fwrite(&studentCount, sizeof(int), 1, studentFile);
    fwrite(students, sizeof(Student), studentCount, studentFile);
    fclose(studentFile);
}

void hashPassword(const char* password, char* hashedPassword) {
    unsigned long hash = 5381;
    int c;
    while ((c = *password++)) {
        hash = ((hash << 5) + hash) + c;
    }
    sprintf(hashedPassword, "%lu", hash);
}

const char* getGrade(float marks) {
    if (marks >= 90) return "A+";
    if (marks >= 80) return "A";
    if (marks >= 70) return "B+";
    if (marks >= 60) return "B";
    if (marks >= 50) return "C+";
    if (marks >= 40) return "C";
    if (marks >= 35) return "D";
    return "NG";
}

float calculateGPA(float marks[], int count) {
    float totalGradePoints = 0.0;
    int subjectsWithMarks = 0;

    for (int i = 0; i < count; i++) {
        float gradePoint;
        if (marks[i] >= 90) gradePoint = 4.0;
        else if (marks[i] >= 80) gradePoint = 3.6;
        else if (marks[i] >= 70) gradePoint = 3.2;
        else if (marks[i] >= 60) gradePoint = 2.8;
        else if (marks[i] >= 50) gradePoint = 2.4;
        else if (marks[i] >= 40) gradePoint = 2.0;
        else if (marks[i] >= 35) gradePoint = 1.6;
        else gradePoint = 0.0;

        totalGradePoints += gradePoint;

        if (marks[i] >= 0) {
            subjectsWithMarks++;
        }
    }
    if (subjectsWithMarks == 0) return 0.0;
    return totalGradePoints / subjectsWithMarks;
}
