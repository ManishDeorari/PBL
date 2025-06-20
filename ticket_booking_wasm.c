#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <emscripten/emscripten.h>

#define MAX_BOOKINGS 100

typedef struct {
    char name[100];
    char email[100];
    char phone[20];
    char date[20];   // New: string date
    int day;
    int slot;
    int seat;
} Booking;

Booking bookings[MAX_BOOKINGS];
int booking_count = 0;

const char* days[] = {"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"};
const char* times[] = {"9:00 AM", "12:00 PM", "3:00 PM", "6:00 PM"};

char result_message[256]; // Shared buffer for response

// ✅ Book a ticket with full validation
EMSCRIPTEN_KEEPALIVE
const char* book_ticket(const char* name, const char* email, const char* phone,
                        const char* date, int day, int slot, int seat) {
    if (booking_count >= MAX_BOOKINGS) {
        snprintf(result_message, sizeof(result_message), "❌ Booking limit reached.");
        return result_message;
    }

    // Check for duplicate seat booking
    for (int i = 0; i < booking_count; i++) {
        if (strcmp(bookings[i].date, date) == 0 &&
            bookings[i].day == day &&
            bookings[i].slot == slot &&
            bookings[i].seat == seat) {
            snprintf(result_message, sizeof(result_message),
                     "❌ Seat %d already booked for %s at %s.", seat, days[day], times[slot]);
            return result_message;
        }
    }

    // Add booking
    strncpy(bookings[booking_count].name, name, sizeof(bookings[booking_count].name));
    strncpy(bookings[booking_count].email, email, sizeof(bookings[booking_count].email));
    strncpy(bookings[booking_count].phone, phone, sizeof(bookings[booking_count].phone));
    strncpy(bookings[booking_count].date, date, sizeof(bookings[booking_count].date));
    bookings[booking_count].day = day;
    bookings[booking_count].slot = slot;
    bookings[booking_count].seat = seat;

    booking_count++;

    snprintf(result_message, sizeof(result_message),
             "✅ Ticket booked for %s on %s (%s %s) at seat %d.",
             name, date, days[day], times[slot], seat);
    return result_message;
}

// ✅ Export bookings to CSV
EMSCRIPTEN_KEEPALIVE
const char* export_bookings_csv() {
    static char csv[8192]; // Ensure enough size
    strcpy(csv, "Name,Email,Phone,Date,Day,Time,Seat\n");

    for (int i = 0; i < booking_count; i++) {
        char line[512];
        snprintf(line, sizeof(line), "\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d\n",
                 bookings[i].name,
                 bookings[i].email,
                 bookings[i].phone,
                 bookings[i].date,
                 days[bookings[i].day],
                 times[bookings[i].slot],
                 bookings[i].seat);
        strcat(csv, line);
    }

    return csv;
}

// ✅ Clear all bookings
EMSCRIPTEN_KEEPALIVE
void clear_bookings() {
    booking_count = 0;
}

// ✅ Print bookings to console (optional debug)
EMSCRIPTEN_KEEPALIVE
void print_bookings() {
    printf("Current Bookings (%d):\n", booking_count);
    for (int i = 0; i < booking_count; i++) {
        printf("-> %s (%s) - %s %s, Seat %d on %s\n",
               bookings[i].name,
               bookings[i].email,
               days[bookings[i].day],
               times[bookings[i].slot],
               bookings[i].seat,
               bookings[i].date);
    }
}
