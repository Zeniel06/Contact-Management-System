#include <stdio.h>
#include <ctype.h>  
#include <string.h> 
#include <stdbool.h>
#include <stdlib.h>

// ANSI escape sequences used for color coding text in terminal output
const char *reset = "\033[0m";  // Reset to default color
const char *red = "\033[1;31m"; // Bright red color
const char *orange = "\033[38;2;255;165;0m"; // Orange color
const char *blue = "\033[1;34m"; // Blue color
const char *green = "\033[1;32m"; // Green color

// Struct representing a contact
// Fields: name , phone number, email
struct Contact {
    char name[52];
    char phoneno[16];
    char email [52];
};

// Global variables: (Persists throughout the execution of the program until user exits)
// Stores the current number of contacts saved in the system 
int noOfContacts = 0;
// Initial size of the dynamic memory allocated to store the contacts saved (Can be dynamically resized)
int contactsSize = 100;
// Pointer to the contacts saved 
// Initialised upon the starting of the program when all contacts are load from file
struct Contact * contacts; // Acts as a "contact list" storing all saved contacts

// Validate phone number entered by user
bool validatePhoneNum(char * phoneNum) {
    // Check length of phone number 
    if (strlen(phoneNum) != 10 && strlen(phoneNum) != 11) {
        return false;
    }
    // Check if all characters are digits
    for (int i = 0; i < strlen(phoneNum); ++i) {
        if (! isdigit(phoneNum[i])) {
            return false;
        }
    }
    // Check if phone number begins with 0 and 1
    if (phoneNum[0] != '0' || phoneNum[1] != '1') {
        return false;
    }
    return true;
}

// Validate name entered by user
bool validateName(char * name) {
    // Check length of name
    if (strlen(name) < 1 || strlen(name) > 50) {
        return false;
    }
    // Check if the first character is an alphabet
    if (! isalpha(name[0])) {  
        return false;
    }
    // Check if intermediate characters are valid
    for (int i = 1; i < strlen(name)-1; ++i) {
        if (! (isalpha(name[i]) || name[i] == ' ' || name[i]== '-')) {
            return false;
        }
    }
    // Check if last character is an alphabet
    if (! isalpha(name[strlen(name) - 1])) {  
        return false;
    }
    return true;
}

// Check for consecutive occurences of a character in a string
bool checkConsec(char c, char * str) { // Used in email validation
    char previous = ' ';
    for (int i = 0; i < strlen(str); ++i) {
        if (str[i] == c) {
            if (str[i] == previous) {
                return false;
            }
        }
        previous = str[i];
    }
    return true;
}

// Validate email entered by user
bool validateEmail(char * email) {
    // Check length of email
    if (strlen(email) < 1 || strlen(email) > 50) {
        return false;
    }
    // Check if email begins with alphabet
    if (! isalpha(email[0])) {
        return false;
    }
    // Check if there is '@' character
    char * atPtr = strchr(email + 1, '@');
    if (atPtr == NULL) {
        return false;
    }
    // Check if last character of local part is alphanumeric
    if (! isalnum(*(atPtr-1)) ) {
        return false;
    }
    // Check if there exists a domain name
    if (strlen(atPtr) == 1) {
        return false;
    }
    // Check if the first character of domain name is alphanumeric
    if (! isalnum(atPtr[1])) {
        return false;
    }
    // Check for at least one dot after the '@'
    if (strchr(atPtr, '.') == NULL) {
        return false;
    }
    // Check if the last character is alphanumeric
    if (! isalnum(email[strlen(email) - 1])) {
        return false;
    }
    // Disallow consecutive . or - or _
    if (! (checkConsec('.', email) && checkConsec('-', email) && checkConsec('_', email))) {
        return false;
    }
    // Check if there are no space characters
    if (strchr(email, ' ') != NULL) {
        return false;
    }
    return true;
}

// rot 47 encryption algorithm (Used to ecrypt the contact details stored in file)
// Shifts a character forward by 47 positions within the range of 94 printable ASCII characters
void rot47(char * data) {
    for (int i = 0; i < strlen(data); i++) {
        if (data[i] >= 33 && data[i] <= 126) {  // Encrypt only characters whose ASCII is between 33 and 126
            data[i] = 33 + ((data[i] - 33 + 47) % 94); // Formula: output = 33 + ((input + 47 -33) mod 94)
        }
    }
}

// Write a contact to file
void writeToFile(struct Contact contact) {
    FILE * f = fopen("contacts.txt", "a"); // Open file mode as append
    rot47(contact.name); // Ecrypt the name 
    fprintf(f, "%s\n", contact.name); // Save the encrypted data to file after encryption
    rot47(contact.phoneno);
    fprintf(f, "%s\n", contact.phoneno);
    rot47(contact.email);
    fprintf(f, "%s\n", contact.email);
    fclose(f); // Close file
}

// Remove newline character at the end of name, phone number and email after a contact is load from file
void removeNewline(struct Contact* contact) {
    // Find the length of the initial segment of the string without '\n' and use that as the index of '\n' to set it to '\0'
    (*contact).name[strcspn((*contact).name, "\n")] = '\0'; 
    (*contact).phoneno[strcspn((*contact).phoneno, "\n")] = '\0';
    (*contact).email[strcspn((*contact).email, "\n")] = '\0';
}

// Prompt user for either 'y' or 'n'(Used to decide whether to continue a specific operation)
bool getDecision (void) {
    char buffer[1024];
    char option;
    while (true) {
        printf("Enter 'y' for yes or 'n' for no: ");
        scanf(" %[^\n]", buffer); 
        if (strlen(buffer) == 1 && (buffer[0] =='n' || buffer[0] == 'y')) {
            option = buffer[0];
            break;
        } else {
            printf("%sInvalid input! Please enter again!\n%s", red, reset);
        }
    }
    return option == 'y';   
}

// Prompt user to enter a name and store it in the buffer
void getName(char * buffer) {
    while (true) { // Loop until the user input a valid name
        printf("Enter the name: \n");
        scanf(" %[^\n]", buffer); 
        if (validateName(buffer)) { 
            break;
        } else {
            printf("%sInvalid name! Please enter again!\n%s", red, reset);
        }
    }
}

// Prompt user to enter a phone number and store it in the buffer
void getPhoneNum(char * buffer) {
    while (true) { // Loop until the user input a valid phone number
        printf("Enter the phone number: \n");
        scanf(" %[^\n]", buffer); 
        if (validatePhoneNum(buffer)) {
            break;
        } else {
            printf("%sInvalid phone number! Please enter again\n%s", red, reset);
        }
    }
}

// Prompt user to enter an email and store it in the buffer
void getEmail(char * buffer) {
    while (true) { // Loop until the user input a valid email
        printf("Enter the email: \n");
        scanf(" %[^\n]", buffer); 
        if (validateEmail(buffer)) {
            break;
        } else {
            printf("%sInvalid email! Please enter again\n%s", red, reset);
        }
    } 
}

// Resize the dynamic memory allocated to store the contacts when the number of contacts reached the limit
void resizeContacts() {
    if (noOfContacts == contactsSize) {
        contactsSize *= 2; // Double the size each time
        contacts = realloc(contacts, contactsSize * sizeof(struct Contact)); // Reallocate dynamic memory to store all contacts saved
    }
}

// Allow user to input new contact details and save the new contact to the contact list
void addContacts() {
    char buffer[1024]; // Used to store the contact fields entered by user
    do {
        struct Contact contact; // Used to hold the new contact input by user and save it to the contact list
        getName(buffer); // Call getName() to prompt user for a name
        strcpy(contact.name, buffer);  // Copy name to name field of the contact struct
        getPhoneNum(buffer); // Call getPhoneNum() to prompt user for a phone number
        strcpy(contact.phoneno, buffer); // Copy phone number to phone number field of the contact struct
        getEmail(buffer); // Call getEmail() to prompt user for an email
        strcpy(contact.email, buffer); // Copy email to email field of the contact struct
        writeToFile(contact); // Write the newly added contact to file
        contacts[noOfContacts] = contact;   // Store the new contact to the contact list so it is visible to the program
        ++noOfContacts;   // Increament noOfContacts after a new contact is added
        printf("%sContact succesfully added!\n%s", green, reset);
        resizeContacts(); // Resize the dynamic memory to store all contacts if needed
        printf("\nDo you want to continue adding?\n");
    } while (getDecision());
}

// Called immediately at the start of the program to load all saved contacts from file to the program
struct Contact * loadContactsFromFile(void) {
    FILE * f = fopen("contacts.txt", "r"); // Open file as read mode
    // Allocate dynamic memory to store all contacts load from file
    // The pointer will be assigned to the global variable "contacts" after the function has been executed
    struct Contact * contacts = malloc(contactsSize * sizeof(struct Contact)); 
    // Loop to load contacts from file by reading three lines each time for the name, phone number and email
    // Loop until fgets() returns NULL indicating end of file
    while (fgets(contacts[noOfContacts].name, sizeof(contacts[noOfContacts].name), f) != NULL) { 
        fgets(contacts[noOfContacts].phoneno, sizeof(contacts[noOfContacts].phoneno), f);
        fgets(contacts[noOfContacts].email, sizeof(contacts[noOfContacts].email), f);
        removeNewline(contacts + noOfContacts); // Remove the newline character '\n' for each field of the contact read from file
        // Decrypt the data
        rot47(contacts[noOfContacts].name);
        rot47(contacts[noOfContacts].phoneno);
        rot47(contacts[noOfContacts].email);
        ++ noOfContacts; // Increament noOfContacts each time a contact is load from file
        if (noOfContacts == contactsSize) { // Resize the dynamic memory allocated to store the contacts when needed
            contactsSize *= 2;
            contacts = realloc(contacts, contactsSize * sizeof(struct Contact));
        }
    }
    fclose(f); // Close file
    return contacts; // Return pointer to dynamic memory storing all the contacts load from file
}

// Used to print out the user guidelines when the user requests to look at it
void userGuidelines(void) {
    printf("=====================================================================================================================\n");
    printf("%s                                                 User Guidelines                                                     %s\n", blue, reset);
    printf("=====================================================================================================================\n");
    printf("%s  Features available:\n%s", orange, reset);
    printf("%s  1. Add Contacts\n%s", orange, reset);
    printf("     - Allows the user to store new contacts into the system.\n");
    printf("     - Contact Format:\n");
    printf("         * Name:         - Must be at most 50 characters long and only contains alphabetic characters, '-' and ' '.\n");
    printf("         * Phone Number: - Must begin with '01' followed by 8 or 9 digits.\n");
    printf("         * Email:        - Must be at most 50 characters long.\n");
    printf("                         - Local part must begin with an alphabet and end with an alphanumeric character.\n");
    printf("                         - Must have exactly one '@' after the local part.\n");
    printf("                         - A domain name must exist after '@', starting with an alphanumeric character.\n");
    printf("                         - The domain part must contain at least one dot '.' .\n");
    printf("                         - No consecutive '.' or '-' or '_' allowed.\n\n");
    printf("%s  2. Delete Contacts\n%s", orange, reset);
    printf("     - Allows the user to search for a contact based on either name, phone number or email and delete it.\n\n");
    printf("%s  3. Display Contacts\n%s", orange, reset);
    printf("     - Displays all contacts currently stored in the system to the user.\n\n");
    printf("%s  4. Sort Contacts\n%s", orange, reset);
    printf("     - Allows the user to sort the contacts based on any of the fields (name, phone number, or email).\n\n");
    printf("%s  5. Search Contacts\n%s", orange, reset);
    printf("     - Allows the user to search for contacts based on any of the fields (name, phone number, or email).\n\n");
    printf("%s  6. Search Contacts by Partial Matches\n%s", orange, reset);
    printf("     - Allows the user to search for contacts based on a specific key.\n");
    printf("     - For example, search for all contacts that begin with a certain letter.\n\n");
    printf("%s  7. Edit Contacts\n%s", orange, reset);
    printf("     - Allows the user to search for a contact based on either name, phone number or email and edit it.\n\n");
    printf("%s  8. Exit\n%s", orange, reset);
    printf("     - Allows the user to exit from the program.\n");
    printf("=====================================================================================================================\n");
}

// Display all of the contacts pointed to by a pointer to the user
void displayContacts(struct Contact * contacts, int noOfContacts) {
    if (noOfContacts == 0) {
        printf("%sNo contacts stored!\n%s", red, reset); // If number of contacts is 0 print no contacts stored
    } else {
        printf("\nContacts List\n");
        printf("|%-10s|%-50s|%-15s|%-50s|\n", "Index", "Name", "Phone Number", "Email"); // Format the header
        for (int i = 0; i < noOfContacts; ++i) {
            printf("|%-10d|", i + 1); // Start at index 1 when displaying the contacts to user
            printf("%-50s|", contacts[i].name); // Display the name
            printf("%-15s|", contacts[i].phoneno); // Display the phone number
            printf("%-50s|\n", contacts[i].email); // Display the email
        }
    }
}

// Start of implementation of sorting feature
// Convert all characters of a string to lower case
char * convertToLower(char * str) {
    char * lowerCaseVersion = malloc((sizeof(char) * strlen(str)) + 1);
    for (int i = 0; i < strlen(str); ++i) {
        lowerCaseVersion[i] = tolower(str[i]);
    }
    lowerCaseVersion[strlen(str)] = '\0';
    return lowerCaseVersion;
}

// Given the left halve and the right halve (of an array), compare the element at the begining of each halves
// sortBy indicates sort by name or phone number or email
// Use strcasecmp to compare two strings, either name, phone number or email depends on sortBy
int cmp(struct Contact* leftHalve, struct Contact* rightHalve, char sortBy) {
    int result; // Stores the result of the comparison
    char * leftHalveLower;
    char * rightHalveLower;
    switch (sortBy) {
        case 'n': // Sort the contacts depending on user's choice
            leftHalveLower = convertToLower((*leftHalve).name);    // Convert the name to lower case for case insensitive comparison
            rightHalveLower = convertToLower((*rightHalve).name);
            result = strcmp(leftHalveLower, rightHalveLower);
            free(leftHalveLower);  // Free the dynamic memory allocated to store the string converted to lower case
            free(rightHalveLower);
            break;
        case 'p':
            result = strcmp((*leftHalve).phoneno, (*rightHalve).phoneno);
            break;
        case 'e':
            leftHalveLower = convertToLower((*leftHalve).email);
            rightHalveLower = convertToLower((*rightHalve).email);
            result = strcmp(leftHalveLower, rightHalveLower);
            free(leftHalveLower);
            free(rightHalveLower);
            break;
    }
    return result;
}

// Merge two sorted halves into a larger sorted array
// Given two halves, compare the item at the begining of each halve, add the smaller on to the array "merged"
// Repeat until one halve is empty and copy all items on the remaining halve to array "merged"
struct Contact* merge(char sortBy, struct Contact* leftHalve, struct Contact* rightHalve, int size1, int size2) {
    struct Contact * merged = malloc(sizeof(struct Contact) * (size1 + size2));
    struct Contact * startOfList = leftHalve;
    int totalSize = size1 + size2;
    struct Contact * startOfMerged = merged;
    int result;
    while (size1 != 0 && size2 != 0) {
        result = cmp(leftHalve, rightHalve, sortBy);
        if (result < 0) {
            *merged = *leftHalve;
            ++ leftHalve;
            -- size1; 
        } else {
            *merged = *rightHalve;
            ++ rightHalve;
            -- size2; 
        }
        ++ merged;
    }

     if (size1 == 0) {
        // Copy all remaining Contact structs to array "merged"
         memcpy(merged, rightHalve, size2 * sizeof(struct Contact)); 
    } else {
         memcpy(merged, leftHalve, size1 * sizeof(struct Contact));
    }
    memcpy(startOfList, startOfMerged, totalSize * sizeof(struct Contact));
    free(startOfMerged);
    return startOfList;
}

// Main merge sort function
struct Contact* mergesort(char sortBy, struct Contact* contacts, int size) {
    if (size == 1) {    // Base case: return the array itself if the size is one
        return contacts;
    }
    // Recursive case
    // Divide the array into two halves and call mergesort function recursively to sort each halve
    int size1 = size/2;    
    int size2 = size - size1;
    struct Contact* leftHalve = mergesort(sortBy, contacts, size1);
    struct Contact* rightHalve = mergesort(sortBy, contacts + (size/2), size2);
    // Finally merge the sorted two halves
    return merge(sortBy, leftHalve, rightHalve, size1, size2);
}

// Main sort function (called by the main function in the menu if user selects this operation to be performed)
// Calls the mergeSort function and write the sorted contacts to file in a new order
void sort(void) {
    if (noOfContacts == 0) { // End the sorting operation directly if there are no contacts to be sorted
        printf("%sNo contacts stored. Unable to perform sorting operation!\n%s", red, reset);
        return;
    }
    char sortBy;
    char buffer[1024];
    while (true) { // Loop until the user inputs a valid choice before sorting
        printf("Select an option to sort the contacts based on it:\n");
        printf("'n'--> name\n");
        printf("'p'--> phone number\n");
        printf("'e'--> email          choice: ");
        scanf(" %[^\n]", buffer); 
        if (strlen(buffer) == 1 && (buffer[0] == 'n' || buffer[0] == 'p' || buffer[0] == 'e')) {
            sortBy = buffer[0];
            break;
        } else {
            printf("%sInvalid option! Please enter again!\n%s", red, reset);
        }
    } 
    mergesort(sortBy, contacts, noOfContacts); // Call mergesort function and start sorting the contacts based on the user's choice
    // Write the sorted contacts back to into the file (overwrites the previous entries)
    FILE * f = fopen("contacts.txt", "w");
    for (int i = 0; i < noOfContacts; ++i) {
        writeToFile(contacts[i]);
    }
    printf("%sContacts sorted!\n%s", green, reset);
    displayContacts(contacts, noOfContacts); // Display the sorted contacts
    fclose(f);
}
// End of implementation of sorting feature

// Prompt user to input a contact field (name, phone number or email)
void getContactField (char * buffer) {
    while (true) { // Loop until user has input a valid contact field
        printf("Enter a field of a contact (name, phone number or email): \n");
        scanf(" %[^\n]", buffer); 
        if (validateEmail(buffer) || validateName(buffer) || validatePhoneNum(buffer)) {
            return;
        }
        printf("%sInvalid input! Please enter again!\n%s", red, reset);
    }
}


// Allow user to search for specific contacts based on any field and delete one or all matching contacts (batch deletion)
void deleteContacts(void) {
    struct Contact temp[noOfContacts]; // Store all contacts that are not deleted temporarily 
    char field[55]; // Store the input field used to search for the contact to be deleted
    int count; // Store the number of contacts remaining (Also acts as index for temp array to store the contacts that are not deleted)
    do {
        if (noOfContacts == 0) { // If no contacts stored, inform user and exit directly
            printf("%sNo contacts stored!\n%s", red, reset);
            return;
        }
        count = 0; // Initialise count to 0
        printf("You can search for the contacts to be deleted by name, phone number or email\n");
        getContactField(field); // Prompt user to input a field and use it to search for the contact to be deleted
        FILE * f = fopen("contacts.txt", "w"); // Open file in write mode
        for (int i = 0; i < noOfContacts; ++i) { // Loop through all of the contacts
            // Check if any field of the contact matches the input field, if matches, the contact will be deleted
            if (strcmp(contacts[i].name, field) == 0 || 
            strcmp(contacts[i].phoneno, field) == 0 || 
            strcmp(contacts[i].email, field) == 0) {  
                // Inform the user that the contact has been deleted
                printf("%s %s %s %shas been deleted successfully%s\n", contacts[i].name, contacts[i].phoneno, contacts[i].email, green, reset);
            } else {
                // If the contact is not the contact to be deleted, store it in temp
                temp[count] = contacts[i];
                ++count; // Increament count each time a contact is stored into temp
                writeToFile(contacts[i]); // Write the contact not deleted back to file
            }
        }
        fclose(f); // Close file
        if (count == noOfContacts) {
            printf("%sNo relevant contacts found!\n%s", red, reset); // If no contacts are deleted, display a message to inform user
        } else {
            // If there are contacts deleted, copy the remaining contacts in temp back into the memory allocated to store the contacts saved
            memcpy(contacts, temp, sizeof(struct Contact) * count); 
            noOfContacts = count; // Reset noOfContacts to count
        }
        printf("\nDo you want to continue deleting?\n");
    } while(getDecision());   
}

// Allow user to search for contacts by combining multiple (at most 3) fields
void searchContacts(void){
    int size; 
    char field[55];
    if (noOfContacts == 0) { // If no contacts stored, inform user and exit directly
        printf("%sNo contacts stored!\n%s", red, reset);
        return;
    }
    do {
        size = 0;
        printf("All contacts with matching fields will be displayed\n");
        struct Contact * matchingContacts = malloc(sizeof(struct Contact) * noOfContacts); // Pointer to all matching contacts
        getContactField(field);
        for (int i = 0; i < noOfContacts; ++i) {
            if (strcmp(contacts[i].name, field) == 0 || 
            strcmp(contacts[i].phoneno, field) == 0 || 
            strcmp(contacts[i].email, field) == 0) {
                matchingContacts[size] = contacts[i];
                ++size;
            }
        }
        if (size == 0) { // If no contacts found, display the message to inform user
            printf("%sNo relevant contacts found!\n%s", red, reset);
        } else { // Or else, inform user that all relevant contacts are found
            printf("%sSuccessfully found all relevant contacts!\n%s", green, reset);
            displayContacts(matchingContacts, size); // Display all contacts found to user
        }
        free(matchingContacts);
        printf("\nDo you want to continue searching?\n");
    } while (getDecision());
}

// Search by partial matching (Bonus feature)
// Allow user to search for contacts by partial matching e.g. all contacts with name begining with a specific key
void partialMatching(void) {
    if (noOfContacts == 0) { // If no contacts stored, inform user and exit directly
        printf("%sNo contacts stored!\n%s", red, reset);
        return;
    }
    do {
        printf("This feature allows user to search for contacts by partial matching on name\n");
        printf("e.g. searching for contacts whose names start with a certain letter\n");
        char key[1024];
        int count = 0;
        struct Contact * matchingContacts = malloc(sizeof(struct Contact) * noOfContacts); // Store all matching contacts
        while (true) { // Validate the key entered by user
            printf("Enter a key of length 1 to 5\n");
            scanf(" %[^\n]", key);
            if (strlen(key) >= 1 && strlen(key) <=5) {
                break;
            }
            printf("%sInvalid key! Please enter again\n!%s", red, reset);
        }
        // Loop through all contacts and find the matching ones
        for (int i = 0; i < noOfContacts; ++i) {
            if (strncmp(contacts[i].name, key, strlen(key)) == 0) {
                matchingContacts[count] = contacts[i];
                ++ count;
            }
        }
        if (count == 0) { // If no contacts found, inform the user
            printf("%sNo relevant contacts found!\n%s", red, reset);
        } else { // Otherwise, call displayContacts to display all contacts found to user
            printf("%sSuccessfully found all relevant contacts!\n%s", green, reset);
            displayContacts(matchingContacts, count);
        }
        free(matchingContacts);
        printf("\nDo you want to continue searching?\n");
    } while (getDecision());
}

// Edit a specific contact
void editContacts(void) {
   if (noOfContacts == 0) { // If no contacts stored, inform the user and exit directly
        printf("%sNo contacts stored!\n%s", red, reset);
        return;
    }
    bool found;
    bool edit;
    char field[55];
    char newData[1024];
    struct Contact oldContact;
    do {
        printf("You can search for a contact to be edited by name, phone number or email\n");
        getContactField(field);
        FILE * f = fopen("contacts.txt", "w");
        found = false;
        for (int i = 0; i < noOfContacts; ++i) { // Loop through all contacts stored
             if (strcmp(contacts[i].name, field) == 0 || // Check if the contact is the targeted contact to be edited
            strcmp(contacts[i].phoneno, field) == 0 || 
            strcmp(contacts[i].email, field) == 0) {
                oldContact = contacts[i];
                // Prompt the user if they want to edit the name and reset the name if necessary
                printf("Do you want to edit the name?\n");
                edit = getDecision();
                if (edit) {
                    getName(newData);
                    strcpy(contacts[i].name, newData);
                }
                // Prompt the user if they want to edit the phone number and reset the phone number if necessary
                printf("Do you want to edit the phone number?\n");
                edit = getDecision();
                if (edit) {
                    getPhoneNum(newData);
                    strcpy(contacts[i].phoneno, newData);
                }
                // Prompt the user if they want to edit the email and reset the email if necessary
                printf("Do you want to edit the email?\n");
                edit = getDecision();
                if (edit) {
                    getEmail(newData);
                    strcpy(contacts[i].email, newData);
                }
                // Display how the contact is being updated
                printf("\033[1;32mContact successfully updated from\033[0m %s %s %s \033[1;32mto\033[0m %s %s %s\n", 
                oldContact.name, oldContact.phoneno, oldContact.email, 
                contacts[i].name, contacts[i].phoneno, contacts[i].email);
                found = true;
            }
            writeToFile(contacts[i]);  
        }
        fclose(f);
        // If no contact is edited, inform the user
        if (found == false) {
            printf("%sNo relevant contact found!\n%s", red, reset);
        }
        printf("\nDo you want to continue editing?\n");
    } while (getDecision());
}

// Called to clear the input buffer
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {  // Discard all characters in the buffer 
    }
}

// Main function that utilizes a do-while loop to print the menu and prompt the user for what operation to be performed
// Only stop when the user chooses to exit
int main(void) {
    contacts = loadContactsFromFile();
    char choice;
    char buffer[1024];
    do {
        printf("\n========================================\n");
        printf("%s                 MENU                   %s\n", blue, reset);
        printf("========================================\n");
        printf("%s 0. User Guidelines                     %s\n", orange, reset);
        printf("%s 1. Add Contacts                        %s\n", orange, reset);
        printf("%s 2. Delete Contacts                     %s\n", orange, reset);
        printf("%s 3. Display Contacts                    %s\n", orange, reset);
        printf("%s 4. Sort Contacts                       %s\n", orange, reset);
        printf("%s 5. Search Contacts                     %s\n", orange, reset);
        printf("%s 6. Search by Partial Matches           %s\n", orange, reset);
        printf("%s 7. Edit Contacts                       %s\n", orange, reset);
        printf("%s 8. Exit                                %s\n", orange, reset);
        printf("========================================\n");

        // Loop until the user input a valid choice
        while (true) {
            printf("Enter your choice (0 to 8): ");
            scanf(" %[^\n]", buffer); 
            if (strlen(buffer) == 1 && buffer[0] >= '0' && buffer[0] <= '8') {
                choice = buffer[0];
                break;
            } else {
                printf("%sInvalid choice! Please enter again!\n%s", red, reset);
            }
        }
            
        // Calls the relevant function depending on the user's choice
        switch (choice)
        {
        case '0':
            userGuidelines();
            break;
        case '1':
            addContacts();
            break;
        case '2':
            deleteContacts();
            break;
        case '3':
            displayContacts(contacts, noOfContacts);
            break;
        case '4':
            sort();
            break;
        case '5':
            searchContacts();
            break;
        case '6':
            partialMatching();
            break;
        case '7':
            editContacts();
            break;
        case '8':
            printf("Exiting program.\n");
            break;
        }
    } while (choice != '8');
    free(contacts);
    return 0;
}