#include "chord.h"
#include <string>
#include <map>
#include <vector>
#include <algorithm>
#include <qstring.h>

Chord::Chord(std::string root, std::string quality)
{
    this->root = root;
    this->quality = quality;
    this->notes.push_back(noteToVal(root));
    for (unsigned int i = 0; i < qualities.at(quality).size(); i++){
        this->notes.push_back((noteToVal(root)+qualities.at(quality)[i])%12);
    }
    std::sort(notes.begin(), notes.end());
}

const std::map<char, int> Chord::naturalNotes = {{'C', 0}, {'D', 2}, {'E', 4}, {'F', 5}, {'G', 7}, {'A', 9}, {'B', 11}};

const std::map<std::string, std::vector<int>> Chord::qualities = {{"", {4,7}}, {"m", {3,7}}, {"dim", {3,6}},
                                                                  {"aug", {4,8}}, {"sus2", {2,7}}, {"sus4", {5,7}},
                                                                  {"7", {4,7,10}}, {"m7", {3,7,10}}};

int Chord::noteToVal(std::string note){
    int val = naturalNotes.at(note[0]);
    if (note.length() == 2){
        if (note[1]=='#'){
            val+=1;
        }
        else if (note[1]=='b'){
            val-=1;
        }
    }
    return val % 12;
}

QString Chord::chartFromFrets(std::vector<int> frets){
    QString chart;
    int min, max;
    unsigned long strings = frets.size();
    std::vector<int> nonzerofrets;
    for (unsigned int i = 0; i < strings; i++){
        int a = frets[i];
        if (a!=0){
            nonzerofrets.push_back(a);
        }
    }
    if (nonzerofrets.empty()){
        min = 1;
        max = 2;
    }
    else{
        min = *std::min_element(nonzerofrets.begin(), nonzerofrets.end());
        max = *std::max_element(nonzerofrets.begin(), nonzerofrets.end());
    }

    if (max<=4||min<=2){
        min = 1;
    }

    QString separator = QString("├") + QString("┼").repeated(int(strings-2)) + QString("┤");
    QString top;
    if (min==1){
        top = QString("┌") + QString("┬").repeated(int(strings-2)) + QString("┐");
    }
    else{
        top = separator;
    }

    chart += top + '\n';
    for (int i = min; i <= max; i++){
        for (unsigned int f = 0; f < frets.size(); f++){
            if (frets[f] == i) {
                chart += QString("Ф");
            }
            else{
                chart += QString("│");
            }
        }
        if (i==min && i!=1){
            chart += QString(" ") + QString::number(i) + QString("fr");
        }
        chart += '\n';
        chart += separator + '\n';
    }
    return chart;
}


struct{
    int range(const std::vector<int> &a) const{
        std::vector<int> zeroes(a.size(), 0);
        if (a==zeroes){
            return 0;
        }
        else{
            int min = 1000000;
            int max = 0;
            for(int i = 0; i < a.size(); i++){
                if (a[i]!=0){
                    if (a[i]>max){max = a[i];}
                    if(a[i]<min){min=a[i];}
                }
            }
            return max-min;
        }

    }
bool operator()(const std::vector<int> &a, const std::vector<int> &b) const{
    return range(a)<range(b);
    }
} cmp;

std::vector<std::vector<int>> Chord::getFrets(std::vector<int> tuning){
    std::vector<std::vector<int>> variants;
    int strings = int(tuning.size());
    if (strings < this->notes.size()){
        throw std::length_error("not enough strings");
    }
    for (int offset = 0; offset < 12; offset++){
        std::vector<int> frets(strings, 0);
        std::vector<int> fr_notes(strings, 0);

        for (int str = 0; str < strings; str++){
            int start = tuning[str];
            int target;
            std::vector<int> candidates;
            for (unsigned int i = 0; i < this->notes.size(); i++){
                if (this->notes[i] >= start){
                    candidates.push_back(this->notes[i]);
                }
            }

            if (!candidates.empty()){
                target = *std::min_element(candidates.begin(), candidates.end());
                frets[str] = target - start;
            }
            else{
                target = this->notes[0];
                frets[str] = 12 - start + target;
            }
            fr_notes[str] = target;
        }

        std::sort(fr_notes.begin(), fr_notes.end());

        std::vector<int>::iterator it;
        std::vector<int> v(strings);
        it=std::set_difference(this->notes.begin(), this->notes.end(), fr_notes.begin(), fr_notes.end(), v.begin());
        v.resize(it-v.begin());

        for (it=v.begin(); it!=v.end(); ++it){
            int target = *it;
            std::vector<int> doubles;
            for(int i = strings-1; i >= 0; i--){
                if (std::count(fr_notes.begin(), fr_notes.end(), fr_notes[i]) > 1){
                    doubles.push_back(i);
                }
            }

            int smallest = 100000;
            int string_to_change = 0;
            for(int i = 0; i < doubles.size(); i++){
                int str = doubles[i];
                int start = tuning[str];
                int fret;
                if (target > start){
                    fret = target - start;
                }
                else{
                    fret = 12 - start + target;
                }
                if (fret < smallest){
                    smallest = fret;
                    string_to_change = str;
                }
            }
            frets[string_to_change] = smallest;
            fr_notes[string_to_change] = target;
        }

        for (int i = 0; i < strings; i++){
            tuning[i] += 1;
            tuning[i] %= 12;
            frets[i] += offset;
        }
        if (std::find(variants.begin(), variants.end(), frets) == variants.end()){
            variants.push_back(frets);
        }

    }

    std::sort(variants.begin(), variants.end(), cmp);
    return variants;
}

std::vector<QString> Chord::getCharts(std::vector<int> tuning){
    std::vector<std::vector<int>> fretss = getFrets(tuning);
    std::vector<QString> charts;
    for (int i = 0; i < fretss.size(); i++){
        charts.push_back(chartFromFrets(fretss[i]));
    }
    return charts;
}
