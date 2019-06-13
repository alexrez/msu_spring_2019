// Выполнить сортировку бинарного файла содержащего числа uint64_t в 2 потока.
// Доступно 8 Мб памяти, больше выделять нельзя ни явно, ни неявно (например, в виде контейнеров),
// при этом файл может быть размером превышающим доступную память. Пространство на диске можно считать бесконечным.
// Сортировку выполнять в новый файл.

#include <iostream>
#include <fstream>
#include <vector>
#include <thread>


void merge(const std::string& source, const std::string& target, const size_t chunksize, const size_t chunks){
    std::ifstream source_file(source, std::ios::binary);
    std::ofstream target_file(target, std::ios::binary);

    source_file.seekg(0, std::ios::end);
    const size_t stop = source_file.tellg();

    size_t i = 1;
    while(i < chunks){
        uint64_t number_1;
        size_t ind_1 = 0;
        const size_t offset_1 = (i - 1) * chunksize * sizeof(uint64_t);
        source_file.seekg(offset_1);
        source_file.read((char *) &number_1, sizeof(number_1));

        uint64_t number_2;
        size_t ind_2 = 0;
        const size_t offset_2 = i * chunksize * sizeof(uint64_t);
        source_file.seekg(offset_2);
        source_file.read((char *) &number_2, sizeof(number_2));

        while(ind_1 < chunksize && ind_2 < chunksize){
            if(number_1 <= number_2){
                target_file.write((char *) &number_1, sizeof(number_1));
                ++ind_1;
                if (ind_1 < chunksize && (offset_1 + ind_1 * sizeof(uint64_t)) < stop){
                    source_file.seekg(offset_1 + ind_1 * sizeof(uint64_t));
                    source_file.read((char *) &number_1, sizeof(number_1));
                }
                else{
                    target_file.write((char *) &number_2, sizeof(number_2));
                    ++ind_2;
                    while(ind_2 < chunksize && (offset_2 + ind_2 * sizeof(uint64_t)) < stop){
                        source_file.seekg(offset_2 + ind_2 * sizeof(uint64_t));
                        source_file.read((char *) &number_2, sizeof(number_2));
                        target_file.write((char *) &number_2, sizeof(number_2));
                        ++ind_2;
                    }
                }
            }
            else {
                target_file.write((char *) &number_2, sizeof(number_2));
                ++ind_2;
                if (ind_2 < chunksize && (offset_2 + ind_2 * sizeof(uint64_t)) < stop){
                    source_file.seekg(offset_2 + ind_2 * sizeof(uint64_t));
                    source_file.read((char *) &number_2, sizeof(number_2));
                }
                else{
                    target_file.write((char *) &number_1, sizeof(number_1));
                    ++ind_1;
                    while(ind_1 < chunksize && (offset_1 + ind_1 * sizeof(uint64_t)) < stop){
                        source_file.seekg(offset_1 + ind_1 * sizeof(uint64_t));
                        source_file.read((char *) &number_1, sizeof(number_1));
                        target_file.write((char *) &number_1, sizeof(number_1));
                        ++ind_1;
                    }
                }
            }
        }
        i += 2;
    }

    if (i == chunks){
        size_t ind = (i - 1) * chunksize * sizeof(uint64_t);
        source_file.seekg(ind);
        while(ind < stop){
            uint64_t number;
            source_file.read((char *) &number, sizeof(number));
            target_file.write((char *) &number, sizeof(number));
            ind += sizeof(uint64_t);
        }
    }

    source_file.close();
    target_file.close();
}

void final_merge(const std::string& file_1, const std::string& file_2, const std::string& target){
    std::ofstream file_to_sort(file_1, std::ios::binary | std::ios::app);
    file_to_sort.seekp(0, std::ios::end);

    const size_t chinksize = file_to_sort.tellp() / sizeof(uint64_t);
    
    std::ifstream file_app(file_2, std::ios::binary);
    file_app.seekg(0, std::ios::end);
    const size_t stop = file_app.tellg();
    file_app.seekg(0, std::ios::beg);
    while(file_app.tellg() < stop){
        uint64_t number;
        file_app.read((char *) &number, sizeof(number));
        file_to_sort.write((char *) &number, sizeof(number));
    }

    file_to_sort.close();
    file_app.close();

    const size_t chunks = 2;    
    
    merge(file_1, target, chinksize, chunks);

    std::remove(file_1.c_str());
    std::remove(file_2.c_str());
}

// сортировка каждого файла
void sortChunks(std::string& name, const size_t bufsize){
    std::ifstream file_to_sort(name, std::ios::binary);
    file_to_sort.seekg(0, std::ios::end);

    const size_t filesize = file_to_sort.tellg();
    size_t chunks = filesize / bufsize;
    chunks += filesize % bufsize ? 1 : 0;
    size_t chinksize = bufsize / sizeof(uint64_t);

    file_to_sort.close();

    std::string mergeFile = "merge_" + name;
    
    while (chunks > 1){
        merge(name, mergeFile, chinksize, chunks);
        chinksize *= 2;
        size_t rem = chunks % 2;
        chunks = chunks / 2 + rem;

        std::string tmp_name = name;
        name = mergeFile;
        mergeFile = tmp_name;
    }

    std::remove(mergeFile.c_str());
}

// разделение файла на 2 части для дальнейшей передачи треду
int makeChunks(std::vector<std::string>& files, const size_t threads, const size_t bufsize){
    std::ifstream file_to_sort(files[0], std::ios::binary);
    if (!file_to_sort)
        return 0;

    file_to_sort.seekg(0, std::ios::end);
    const size_t filesize = file_to_sort.tellg();
    const size_t tempSize = filesize / threads;
    const size_t middle = tempSize > (filesize - tempSize) ? tempSize : filesize - tempSize;
    const size_t offset = middle - middle % sizeof(uint64_t);

    file_to_sort.seekg(0, std::ios::beg);

    for (size_t i = 0; i < threads; ++i){
        std::ofstream out(files[i + 1], std::ios::binary);
        if (!out)
            return i + 1;

        const size_t stop = file_to_sort.tellg() < offset ? offset : filesize;

        while(file_to_sort.tellg() < stop){
            std::vector<uint64_t> seq_to_sort;
            for(size_t j = 0; j < bufsize / sizeof(uint64_t) && file_to_sort.tellg() < stop; ++j){
                uint64_t number;
                file_to_sort.read((char *) &number, sizeof(number));
                seq_to_sort.push_back(number);
            }
            std::sort(seq_to_sort.begin(), seq_to_sort.end());
            
            for (auto c: seq_to_sort){
                out.write((char *)&c, sizeof(c)); 
            }
        }
    }
    return 3;
}

int main(int argc, char* argv[]){
    if (argc != 2)
    {
        std::cerr << "Usage:\n";
        std::cerr << "    ./a.out file_to_sort_name\n";
        return 1;
    }

    const size_t numberOfThreads = 2;
    const size_t RAM = 8*1024*1024;
    const size_t threadRAM = RAM / numberOfThreads;

    std::vector<std::string> filenames;
    std::string tmp_file_1 = "tmp_1.bin";
    std::string tmp_file_2 = "tmp_2.bin";

    filenames.push_back((std::string) argv[1]);
    filenames.push_back(tmp_file_1);
    filenames.push_back(tmp_file_2);

    int err = makeChunks(filenames, numberOfThreads, threadRAM);
    if (err == 3){
        std::thread sortingData1(sortChunks, ref(filenames[1]), threadRAM);
        std::thread sortingData2(sortChunks, ref(filenames[2]), threadRAM);

        sortingData1.join();
        sortingData2.join();
    }
    else {
        std::cerr << "Can't open " << filenames[err] << "\n";
        return 1;
    }
    
    std::string sorted_data = "sorted_data.bin";
    final_merge(filenames[1], filenames[2], sorted_data);    

    return 0;
}