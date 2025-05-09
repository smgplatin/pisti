#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define DECK_SIZE 52
#define HAND_SIZE 4
#define MAX_TAKEN_CARDS 52

typedef struct {
    char *suit;
    int value;
} Card;

typedef struct {
    char *isim;
    Card hand[HAND_SIZE];
    Card kazandiklari[MAX_TAKEN_CARDS];
    int kazandik_sayisi;
    int puan;
    int pistiSayisi;
} Oyuncu;

const char *suits[] = {"Kupa", "Karo", "Maca", "Sinek"};
const char *values[] = {"", "A", "2", "3", "4", "5", "6", "7", "8", "9", "10", "J", "Q", "K"};

void desteOlustur(Card *deck) {
    int index = 0;
    for (int s = 0; s < 4; s++) {
        for (int v = 1; v <= 13; v++) {
            deck[index].suit = (char *)suits[s];
            deck[index].value = v;
            index++;
        }
    }
}

void desteKaristir(Card *deck) {
    for (int i = 0; i < DECK_SIZE; i++) {
        int j = rand() % DECK_SIZE;
        Card temp = deck[i];
        deck[i] = deck[j];
        deck[j] = temp;
    }
}

void desteKes(Card *deck) {
    int kesNoktasi;
    do {
        printf("Kacinci karttan kesmek istersiniz (1-%d arasi): ", DECK_SIZE - 1);
        scanf("%d", &kesNoktasi);
    } while (kesNoktasi <= 0 || kesNoktasi >= DECK_SIZE);

    Card tempDeck[DECK_SIZE];
    int index = 0;
    for (int i = kesNoktasi; i < DECK_SIZE; i++) {
        tempDeck[index++] = deck[i];
    }
    for (int i = 0; i < kesNoktasi; i++) {
        tempDeck[index++] = deck[i];
    }
    memcpy(deck, tempDeck, sizeof(Card) * DECK_SIZE);
}

void kartYazdir(Card c) {
    printf("[%s %s]", c.suit, values[c.value]);
}

void kartlariGoster(Card *cards, int count, int goster) {
    for (int i = 0; i < count; i++) {
        if (goster || i == 3) {
            kartYazdir(cards[i]);
        } else {
            printf("[X]");
        }
    }
    printf("\n");
}

void kartEkle(Oyuncu *oyuncu, Card *cards, int count) {
    printf("********* %s kartlari aldi:\n", oyuncu->isim);
    for (int i = 0; i < count; i++) {
        oyuncu->kazandiklari[oyuncu->kazandik_sayisi++] = cards[i];
        kartYazdir(cards[i]);
    }
    printf("\n\n");
}

int puanHesapla(Oyuncu *oyuncu) {
    int puan = 0;
    for (int i = 0; i < oyuncu->kazandik_sayisi; i++) {
        Card c = oyuncu->kazandiklari[i];
        if (c.value == 1) puan += 1;
        if (c.value == 11) puan += 1;
        if (c.value == 10 && strcmp(c.suit, "Karo") == 0) puan += 3;
        if (c.value == 2 && strcmp(c.suit, "Sinek") == 0) puan += 2;
    }
    puan += oyuncu->pistiSayisi * 10;
    return puan;
}

void elGoster(Card *hand) {
    int num = 1;
    for (int i = 0; i < HAND_SIZE; i++) {
        if (hand[i].value != -1) {
            printf("%d: ", num);
            kartYazdir(hand[i]);
            printf("\n");
            num++;
        }
    }
}

int kartSec(Card *hand) {
    int aktifKartlar[HAND_SIZE];
    int secim;
    int num = 1;
    for (int i = 0; i < HAND_SIZE; i++) {
        if (hand[i].value != -1) {
            aktifKartlar[num - 1] = i;
            num++;
        }
    }
    do {
        printf("Hangi karti oynamak istersiniz (1-%d): ", num - 1);
        scanf("%d", &secim);
    } while (secim < 1 || secim >= num);
    return aktifKartlar[secim - 1];
}

void enFazlaKartPuani(Oyuncu *oyuncu1, Oyuncu *oyuncu2) {
    if (oyuncu1->kazandik_sayisi > oyuncu2->kazandik_sayisi) {
        oyuncu1->puan += 3;
        printf("\nEn fazla karti Oyuncu topladi. +3 puan\n");
    } else if (oyuncu2->kazandik_sayisi > oyuncu1->kazandik_sayisi) {
        oyuncu2->puan += 3;
        printf("\nEn fazla karti Bilgisayar topladi. +3 puan\n");
    }
}

int oyunSonuKartlariGoster(Card *masa, int masaIndex, Oyuncu *oyuncu, Oyuncu *bilgisayar) {
    if (masaIndex > 0) {
        if (oyuncu->kazandik_sayisi == 3 || bilgisayar->kazandik_sayisi == 3) {
            printf("\nOyundu Sonu: ");
            kartlariGoster(masa, masaIndex, 1);
            return 1;
        }
    }
    return 0;
}

int main() {
    srand(time(NULL));

    Card deck[DECK_SIZE];
    Oyuncu oyuncu = {"Oyuncu", {}, {}, 0, 0, 0};
    Oyuncu bilgisayar = {"Bilgisayar", {}, {}, 0, 0, 0};

    desteOlustur(deck);
    desteKaristir(deck);
    desteKes(deck);

    Card masa[52];
    int masaIndex = 0;
    int oyuncuSirada = 1;

    printf("\nMasaya dagitilan kartlar: ");
    for (int i = 0; i < 4; i++) {
        masa[masaIndex++] = deck[i];
    }
    kartlariGoster(masa, 4, 0);

    int index = 4;
    while (index < DECK_SIZE) {
        for (int i = 0; i < HAND_SIZE; i++) {
            oyuncu.hand[i].value = -1;
            bilgisayar.hand[i].value = -1;
        }

        for (int i = 0; i < HAND_SIZE; i++) {
            oyuncu.hand[i] = deck[index++];
            bilgisayar.hand[i] = deck[index++];
        }

        for (int i = 0; i < HAND_SIZE; i++) {
            printf("\n--- Yeni Tur ---\n\n");
            Card oyuncuKart, botKart;

            if (oyuncuSirada) {
                printf("Elinizdeki kartlar:\n");
                elGoster(oyuncu.hand);

                int secim = kartSec(oyuncu.hand);
                oyuncuKart = oyuncu.hand[secim];
                oyuncu.hand[secim].value = -1;

                printf("Oyuncu kart atti:\n     \n");
                kartYazdir(oyuncuKart);
                printf("\n     \n");

                masa[masaIndex++] = oyuncuKart;

                if (masaIndex >= 2 && masa[masaIndex - 2].value == oyuncuKart.value) {
                    if (masaIndex == 2) {
                        printf("********* PISTI!!! *********\n");
                        oyuncu.pistiSayisi++;
                    }
                    kartEkle(&oyuncu, masa, masaIndex);
                    masaIndex = 0;
                    oyuncuSirada = 0;
                    continue;
                } else if (oyuncuKart.value == 11 && masaIndex > 1) {
                    printf("***** Vale ile tum kartlar alindi! *****\n");
                    kartEkle(&oyuncu, masa, masaIndex);
                    masaIndex = 0;
                    oyuncuSirada = 0;
                    continue;
                }

                botKart = bilgisayar.hand[i];
                printf("Bilgisayar kart atti:\n     \n");
                kartYazdir(botKart);
                printf("\n     \n");
                masa[masaIndex++] = botKart;

                if (masaIndex >= 2 && masa[masaIndex - 2].value == botKart.value) {
                    if (masaIndex == 2) {
                        printf("********* PISTI!!! *********\n");
                        bilgisayar.pistiSayisi++;
                    }
                    kartEkle(&bilgisayar, masa, masaIndex);
                    masaIndex = 0;
                    oyuncuSirada = 1;
                    continue;
                } else if (botKart.value == 11 && masaIndex > 1) {
                    printf("***** Vale ile tum kartlar alindi! *****\n");
                    kartEkle(&bilgisayar, masa, masaIndex);
                    masaIndex = 0;
                    oyuncuSirada = 1;
                    continue;
                }
            } else {
                // BOT ÖNCE
                botKart = bilgisayar.hand[i];
                printf("Bilgisayar kart atti:\n     \n");
                kartYazdir(botKart);
                printf("\n     \n");
                masa[masaIndex++] = botKart;

                if (masaIndex >= 2 && masa[masaIndex - 2].value == botKart.value) {
                    if (masaIndex == 2) {
                        printf("********* PISTI!!! *********\n");
                        bilgisayar.pistiSayisi++;
                    }
                    kartEkle(&bilgisayar, masa, masaIndex);
                    masaIndex = 0;
                    oyuncuSirada = 1;
                    continue;
                } else if (botKart.value == 11 && masaIndex > 1) {
                    printf("***** Vale ile tum kartlar alindi! *****\n");
                    kartEkle(&bilgisayar, masa, masaIndex);
                    masaIndex = 0;
                    oyuncuSirada = 1;
                    continue;
                }

                printf("Elinizdeki kartlar:\n");
                elGoster(oyuncu.hand);

                int secim = kartSec(oyuncu.hand);
                oyuncuKart = oyuncu.hand[secim];
                oyuncu.hand[secim].value = -1;

                printf("Oyuncu kart atti:\n     \n");
                kartYazdir(oyuncuKart);
                printf("\n     \n");

                masa[masaIndex++] = oyuncuKart;

                if (masaIndex >= 2 && masa[masaIndex - 2].value == oyuncuKart.value) {
                    if (masaIndex == 2) {
                        printf("********* PISTI!!! *********\n");
                        oyuncu.pistiSayisi++;
                    }
                    kartEkle(&oyuncu, masa, masaIndex);
                    masaIndex = 0;
                    oyuncuSirada = 0;
                    continue;
                } else if (oyuncuKart.value == 11 && masaIndex > 1) {
                    printf("***** Vale ile tum kartlar alindi! *****\n");
                    kartEkle(&oyuncu, masa, masaIndex);
                    masaIndex = 0;
                    oyuncuSirada = 0;
                    continue;
                }
            }
        }
    }

    if (masaIndex > 0) {
        if (oyuncuSirada) {
            kartEkle(&bilgisayar, masa, masaIndex);
        } else {
            kartEkle(&oyuncu, masa, masaIndex);
        }
        masaIndex = 0;
    }

    oyuncu.puan = puanHesapla(&oyuncu);
    bilgisayar.puan = puanHesapla(&bilgisayar);

    enFazlaKartPuani(&oyuncu, &bilgisayar);

    printf("\nSkorlar:\n");
    printf("Oyuncu: %d (Pisti: %d)\n", oyuncu.puan, oyuncu.pistiSayisi);
    printf("Bilgisayar: %d (Pisti: %d)\n", bilgisayar.puan, bilgisayar.pistiSayisi);

    if (oyuncu.puan > bilgisayar.puan) {
        printf("Oyunu Oyuncu kazandi!\n");
    } else if (oyuncu.puan < bilgisayar.puan) {
        printf("Oyunu Bilgisayar kazandi!\n");
    } else {
        printf("Oyun berabere!\n");
    }

    return 0;
}