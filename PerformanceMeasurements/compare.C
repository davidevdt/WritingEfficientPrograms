// bool compare(const char* s1, const char* s2, unsigned int l) {
//     if (s1 == s2) return false; 
//     for (unsigned int i1 = 0, i2 = 0; i1 < l; ++i1, ++i2) {
//         if (s1[i1] != s2[i2]) return s1[i1] > s2[i2]; 
//     }
//     return false; 
// }

// Faster version, two changes: 
// - no parameter l passed 
// - using int instead of unsigned int for the index variables 
bool compare(const char* s1, const char* s2) {
    if (s1 == s2) return false; 
    for (unsigned int i1 = 0, i2 = 0;; ++i1, ++i2) {
        if (s1[i1] != s2[i2]) return s1[i1] > s2[i2]; 
    }
    return false; 
}