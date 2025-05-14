#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <thread>
#include <ctime>

using namespace std;

enum TicketCategory { VIP = 1, ECONOMY, STUDENT }; 

struct Ticket {
    string name;
    TicketCategory category;
    string email;
    string phone;
    string booking_time;
    bool isBooked;
    string cancellation_time;
    
    Ticket(string n, TicketCategory c, string e, string p)
        : name(n), category(c), email(e), phone(p), isBooked(false) {
            auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
            booking_time = ctime(&now);
    }
};

struct Node {
    Ticket ticket;
    Node* next;
    Node(Ticket t) : ticket(t), next(nullptr) {}
};

class TicketQueue {
private:
    Node* head;

public:
    TicketQueue() : head(nullptr) {}

    bool isEmpty() {
        return head == nullptr;
    }

    // Insert ticket
    void insertTicket(Ticket ticket) {
        Node* newNode = new Node(ticket);
        
        if (isEmpty() || ticket.category < head->ticket.category) { 
            newNode->next = head;
            head = newNode;
            return;
        }
        
        Node* temp = head;
        while (temp->next && temp->next->ticket.category <= ticket.category) {
            temp = temp->next;
        }
        newNode->next = temp->next;
        temp->next = newNode;
    }

    // Process bookings
    void processBooking() {
        if (isEmpty()) {
            cout << "No tickets left to process.\n";
            return;
        }
        cout << "Processing ticket for: " << head->ticket.name << " (" << (head->ticket.category == VIP ? "VIP" : "Regular") << ")\n";
        head->ticket.isBooked = true;
        head = head->next;  
    }

    void cancelBooking(string customerName) {
        Node* temp = head;
        Node* prev = nullptr;
        
        while (temp) {
            if (temp->ticket.name == customerName && temp->ticket.isBooked) {
                temp->ticket.isBooked = false;
                auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
                temp->ticket.cancellation_time = ctime(&now);
                cout << "Booking cancelled for: " << customerName << endl;
                return;
            }
            prev = temp;
            temp = temp->next;
        }
        cout << "No booked ticket found for " << customerName << endl;
    }

    void modifyBooking(string oldName, string newName, TicketCategory newCategory) {
        Node* temp = head;
        while (temp) {
            if (temp->ticket.name == oldName && temp->ticket.isBooked) {
                temp->ticket.name = newName;
                temp->ticket.category = newCategory;
                cout << "Booking modified: " << oldName << " to " << newName << endl;
                return;
            }
            temp = temp->next;
        }
        cout << "Booking not found for " << oldName << endl;
    }

    void showBookings() {
        if (isEmpty()) {
            cout << "No bookings available.\n";
            return;
        }
        Node* temp = head;
        while (temp) {
            cout << temp->ticket.name << " (" << (temp->ticket.category == VIP ? "VIP" : "Regular") << "), ";
            if (temp->ticket.isBooked)
                cout << "Booked at: " << temp->ticket.booking_time << "\n";
            else
                cout << "Not booked yet\n";
            temp = temp->next;
        }
    }

    ~TicketQueue() {
        while (head) {
            Node* temp = head;
            head = head->next;
            delete temp;
        }
    }
};

void logTransaction(const string& logMessage) {
    ofstream logFile("C:\\Users\\Administrator\\Documents\\projects\\dsa\\ticket_log.txt", ios::app);
    if (logFile.is_open()) {
        auto now = chrono::system_clock::to_time_t(chrono::system_clock::now());
        logFile << ctime(&now) << ": " << logMessage << "\n";
        logFile.close();
    } else {
        cout << "Error opening log file!" << endl;
    }
}

void showMenu() {
    cout << "\nTicket Booking System Menu\n";
    cout << "1. Book a Ticket\n";
    cout << "2. View Booked Tickets\n";
    cout << "3. Cancel a Booking\n";
    cout << "4. Modify a Booking\n";
    cout << "5. Process Next Booking\n";
    cout << "6. Exit\n";
    cout << "Enter your choice: ";
}

void handleBooking(TicketQueue& queue) {
    string name, email, phone;
    int categoryChoice;
    TicketCategory category;

    cout << "Enter your name: ";
    cin.ignore();
    getline(cin, name);
    cout << "Enter your email: ";
    getline(cin, email);
    cout << "Enter your phone number: ";
    getline(cin, phone);
    cout << "Select ticket category (1: VIP, 2: Economy, 3: Student): ";
    cin >> categoryChoice;

    // Map choice to category
    switch (categoryChoice) {
        case 1: category = VIP; break;
        case 2: category = ECONOMY; break;
        case 3: category = STUDENT; break;
        default: 
            cout << "Invalid choice, assigning Economy by default.\n";
            category = ECONOMY; break;
    }

    Ticket ticket(name, category, email, phone);
    queue.insertTicket(ticket);
    logTransaction("Booked ticket for " + name + " (" + to_string(categoryChoice) + ")");
    cout << "Ticket booked successfully for " << name << " in " << (category == VIP ? "VIP" : category == ECONOMY ? "Economy" : "Student") << " category.\n";
}

void handleCancelBooking(TicketQueue& queue) {
    string customerName;
    cout << "Enter the name of the customer whose booking you want to cancel: ";
    cin.ignore();
    getline(cin, customerName);
    queue.cancelBooking(customerName);
    logTransaction("Cancelled booking for " + customerName);
}

void handleModifyBooking(TicketQueue& queue) {
    string oldName, newName;
    int categoryChoice;
    TicketCategory newCategory;

    cout << "Enter the name of the customer whose booking you want to modify: ";
    cin.ignore();
    getline(cin, oldName);
    cout << "Enter the new name: ";
    getline(cin, newName);
    cout << "Select new ticket category (1: VIP, 2: Economy, 3: Student): ";
    cin >> categoryChoice;

    switch (categoryChoice) {
        case 1: newCategory = VIP; break;
        case 2: newCategory = ECONOMY; break;
        case 3: newCategory = STUDENT; break;
        default: 
            cout << "Invalid choice, assigning Economy by default.\n";
            newCategory = ECONOMY; break;
    }

    queue.modifyBooking(oldName, newName, newCategory);
    logTransaction("Modified booking for " + oldName + " to " + newName);
}

void simulateBookingSystem() {
    TicketQueue queue;
    int choice;

    while (true) {
        showMenu();
        cin >> choice;
        
        switch (choice) {
            case 1:
                handleBooking(queue);
                break;
            case 2:
                queue.showBookings();
                break;
            case 3:
                handleCancelBooking(queue);
                break;
            case 4:
                handleModifyBooking(queue);
                break;
            case 5:
                queue.processBooking();
                break;
            case 6:
                cout << "Exiting the system.\n";
                return;
            default:
                cout << "Invalid choice! Please try again.\n";
        }
    }
}

int main() {
    simulateBookingSystem();
    return 0;
}
