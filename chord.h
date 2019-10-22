#ifndef CHORD_H
#define CHORD_H
#include <string>
#include <map>
#include <vector>
#include <qstring.h>

class Chord
{
private:
    std::string root;
    std::string quality;
    static const std::map<char, int> naturalNotes;
    static const std::map<std::string, std::vector<int>> qualities;
    static QString chartFromFrets(std::vector<int> frets);
    std::vector<std::vector<int>> getFrets(std::vector<int> tuning);
    std::vector<int> notes;

public:
    Chord(std::string root, std::string quality);
    static int noteToVal(std::string note);
    std::vector<QString> getCharts(std::vector<int> tuning);
};

#endif // CHORD_H
