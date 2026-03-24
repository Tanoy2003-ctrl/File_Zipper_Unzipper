#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    unsigned char data;
    unsigned freq;
    struct Node *left, *right;
} Node;

typedef struct MinHeap {
    unsigned size;
    Node** array;
} MinHeap;

Node* newNode(unsigned char data, unsigned freq) {
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1, right = 2 * idx + 2;
    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;
    if (smallest != idx) {
        Node* t = minHeap->array[smallest];
        minHeap->array[smallest] = minHeap->array[idx];
        minHeap->array[idx] = t;
        minHeapify(minHeap, smallest);
    }
}

Node* extractMin(MinHeap* minHeap) {
    Node* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);
    return temp;
}

void insertMinHeap(MinHeap* minHeap, Node* node) {
    ++minHeap->size;
    int i = minHeap->size - 1;
    while (i && node->freq < minHeap->array[(i - 1) / 2]->freq) {
        minHeap->array[i] = minHeap->array[(i - 1) / 2];
        i = (i - 1) / 2;
    }
    minHeap->array[i] = node;
}

Node* buildHuffmanTree(unsigned int freq[]) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->array = (Node**)malloc(256 * sizeof(Node*));
    
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) insertMinHeap(minHeap, newNode((unsigned char)i, freq[i]));
    }

    while (minHeap->size != 1) {
        Node *left = extractMin(minHeap);
        Node *right = extractMin(minHeap);
        Node *top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

void decompressFile(const char* inputFilename, const char* outputFilename) {
    FILE *in = fopen(inputFilename, "rb");
    if (!in) { printf("Compressed file not found!\n"); return; }

    unsigned int freq[256];
    fread(freq, sizeof(unsigned int), 256, in);

    Node* root = buildHuffmanTree(freq);
    FILE *out = fopen(outputFilename, "wb");
    
    Node* curr = root;
    unsigned char byte;
    unsigned int totalChars = root->freq;
    unsigned int count = 0;

    while (fread(&byte, 1, 1, in) && count < totalChars) {
        for (int i = 7; i >= 0 && count < totalChars; i--) {
            if ((byte >> i) & 1) curr = curr->right;
            else curr = curr->left;

            if (!curr->left && !curr->right) {
                fwrite(&(curr->data), 1, 1, out);
                count++;
                curr = root;
            }
        }
    }
    fclose(in); fclose(out);
    printf("Decompression Complete: %s created.\n", outputFilename);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <compressed_file.huff> <output_file>\n", argv[0]);
        return 1;
    }
    decompressFile(argv[1], argv[2]);
    return 0;
}