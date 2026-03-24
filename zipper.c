#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 256

// 1. Define the Tree Node
typedef struct Node {
    unsigned char data;
    unsigned freq;
    struct Node *left, *right;
} Node;

// 2. Define the Min-Heap (Priority Queue)
typedef struct MinHeap {
    unsigned size;
    unsigned capacity;
    Node** array;
} MinHeap;

Node* newNode(unsigned char data, unsigned freq) {
    Node* temp = (Node*)malloc(sizeof(Node));
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (Node**)malloc(minHeap->capacity * sizeof(Node*));
    return minHeap;
}

void swapNodes(Node** a, Node** b) {
    Node* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->freq < minHeap->array[smallest]->freq)
        smallest = left;
    if (right < minHeap->size && minHeap->array[right]->freq < minHeap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swapNodes(&minHeap->array[smallest], &minHeap->array[idx]);
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

// 3. Build Huffman Tree
Node* buildHuffmanTree(unsigned char data[], unsigned freq[], int size) {
    Node *left, *right, *top;
    MinHeap* minHeap = createMinHeap(size);
    for (int i = 0; i < size; ++i)
        insertMinHeap(minHeap, newNode(data[i], freq[i]));

    while (minHeap->size != 1) {
        left = extractMin(minHeap);
        right = extractMin(minHeap);
        top = newNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;
        insertMinHeap(minHeap, top);
    }
    return extractMin(minHeap);
}

// 4. Generate Codes and Store in a Table
void storeCodes(Node* root, int arr[], int top, char* codes[]) {
    if (root->left) {
        arr[top] = 0;
        storeCodes(root->left, arr, top + 1, codes);
    }
    if (root->right) {
        arr[top] = 1;
        storeCodes(root->right, arr, top + 1, codes);
    }
    if (!(root->left) && !(root->right)) {
        codes[root->data] = (char*)malloc(top + 1);
        for (int i = 0; i < top; i++) codes[root->data][i] = arr[i] + '0';
        codes[root->data][top] = '\0';
    }
}

// 5. Main Compression Logic
void compressFile(const char* inputFilename, const char* outputFilename) {
    FILE *in = fopen(inputFilename, "rb");
    if (!in) { printf("File not found!\n"); return; }

    unsigned int freq[256] = {0};
    unsigned char ch;
    while (fread(&ch, 1, 1, in)) freq[ch]++;
    rewind(in);

    unsigned char data[256];
    unsigned f[256];
    int size = 0;
    for (int i = 0; i < 256; i++) {
        if (freq[i] > 0) {
            data[size] = (unsigned char)i;
            f[size] = freq[i];
            size++;
        }
    }

    Node* root = buildHuffmanTree(data, f, size);
    int arr[MAX_TREE_HT], top = 0;
    char* codes[256] = {0};
    storeCodes(root, arr, top, codes);

    FILE *out = fopen(outputFilename, "wb");
    // Write frequency table as header (Simplified)
    fwrite(freq, sizeof(unsigned int), 256, out);

    unsigned char buffer = 0;
    int bitCount = 0;
    while (fread(&ch, 1, 1, in)) {
        char *code = codes[ch];
        for (int i = 0; code[i] != '\0'; i++) {
            buffer <<= 1;
            if (code[i] == '1') buffer |= 1;
            bitCount++;
            if (bitCount == 8) {
                fwrite(&buffer, 1, 1, out);
                buffer = 0;
                bitCount = 0;
            }
        }
    }
    // Final padding
    if (bitCount > 0) {
        buffer <<= (8 - bitCount);
        fwrite(&buffer, 1, 1, out);
    }

    fclose(in);
    fclose(out);
    printf("Compression Complete: %s created.\n", outputFilename);
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        printf("Usage: %s <input_file> <output_file.huff>\n", argv[0]);
        return 1;
    }
    compressFile(argv[1], argv[2]);
    return 0;
}