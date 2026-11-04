#pragma once
struct MTSClient {};
inline bool MTS_HasMaster(MTSClient*) { return false; }
inline double MTS_NoteToFrequency(MTSClient*, int, int) { return 0.0; }
