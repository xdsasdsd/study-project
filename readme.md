<div align="center">
Search Engine
</div>
<div align="center">

  <img src="https://img.shields.io/badge/c%2B%2B-%2300599C.svg?style=for-the-badge&logo=c%2B%2B&logoColor=white" alt="C++"/>
  <img src="https://img.shields.io/badge/CMake-%23008FBA.svg?style=for-the-badge&logo=cmake&logoColor=white" alt="CMake"/>
  <img src="https://img.shields.io/badge/JSON-5E5C5C?style=for-the-badge&logo=json&logoColor=white" alt="JSON"/>
</div>

<div align="center">
Search Engine — локальная поисковая система с индексацией текстовых документов и выполнением поисковых запросов. Система построена по принципу инвертированного индекса и позволяет ранжировать результаты поиска по релевантности.

Проект демонстрирует работу основных концепций поисковой системы:


Индексация текстовых документов,

Построение частотного словаря,

Параллельная обработка документов,

Ранжирование поисковых результатов,

Обработка поисковых запросов в интерактивном режиме.
</div>


<div align="center">
🚀 Функциональность

Индексация документов: многопоточная обработка документов для создания инвертированного индекса

Поиск по запросам: поддержка как статичных запросов из JSON-файла, так и интерактивных запросов из консоли

Ранжирование результатов: результаты сортируются по относительной релевантности документов

Анализ частоты слов: статистика по частоте встречаемости слов в документах

Сравнение слов: сравнение частоты встречаемости различных слов

Интерактивный интерфейс: консольный интерфейс с поддержкой различных команд
</div>

<div align="center">
⚙️ Установка и запуск
Системные требования

C++17 или выше

CMake 3.12 или выше

Библиотека nlohmann_json (устанавливается автоматически через CMake)

Сборка проекта
bash

git clone https://github.com/yourusername/search_engine.git
cd search_engine
mkdir build && cd build
cmake ..
make
Конфигурация
Перед запуском необходимо создать файл config.json в корне проекта:

json

{
"config": {
"name": "SearchEngine",
"version": "0.1",
"max_responses": 5
},
"files": [
"../resources/file001.txt",
"../resources/file002.txt",
"../resources/file003.txt",
"../resources/file004.txt"
]
}
Для поисковых запросов создайте файл requests.json:

json

{
"requests": [
"some words to search",
"another search query"
]
}
</div>

<div align="center">
Запуск
Из директории сборки:

bash

./search_engine
🎮 Использование
После запуска программы вы увидите приветствие и информацию о поисковой системе:

code


SEARCH ENGINE

Type 'help' for a list of commands
Search engine: SearchEngine v0.1
Indexing 4 documents...
Indexing completed successfully
Доступные команды

help - Список всех команд

index - Повторная индексация документов

search <запрос> - Поиск документов по запросу

word <слово> - Статистика по заданному слову

find <слово> [предел] - Поиск документов по слову с ограничением результатов

compare <слово1> <слово2> - Сравнение частоты встречаемости двух слов

stats - Показать статистику индекса

process - Обработать все запросы из файла requests.json

exit - Выход из программы
</div>

<div align="center">
Примеры использования
Поиск по запросу
code

> search artificial intelligence
Результат:

code

SEARCH RESULTS FOR: artificial intelligence

Found 3 document(s) in 12 ms
Doc ID     Relevance  Content Preview
----------------------------------------------------------------------
         2       1.000000  Artificial intelligence (AI) is intelligence...
         0       0.750000  Machine learning is a branch of artificial i...
         3       0.500000  Data mining combines statistics and artifici...
Статистика по слову
code

> word intelligence
Результат:

code


WORD STATISTICS: intelligence

Word: intelligence
Found in 3 document(s)
Total occurrences: 7

    Doc ID     Count  Content Preview
----------------------------------------------------------------------
         2         3  Artificial intelligence (AI) is intelligence...
         0         3  Machine learning is a branch of artificial i...
         3         1  Data mining combines statistics and artifici...
Сравнение слов
code

> compare data algorithm
Результат:

code


COMPARING WORDS: data vs algorithm

Word 'data' statistics:
Documents: 4
Total occurrences: 15
Word 'algorithm' statistics:
Documents: 3
Total occurrences: 8
Both words appear in 5 unique document(s)
Documents containing both words: 3
Frequency ratio (data/algorithm): 1.88
</div>

<div align="center">
🏗️ Архитектура проекта
Проект построен на основе трех основных компонентов:


ConverterJSON - обеспечивает работу с конфигурационными файлами и преобразование данных в формат JSON

InvertedIndex - создает и управляет инвертированным индексом для быстрого поиска

SearchServer - обрабатывает поисковые запросы и ранжирует результаты

Структура проекта
code

search_engine/
├── CMakeLists.txt
├── config.json
├── requests.json
├── include/
│   ├── ConverterJSON.h
│   ├── InvertedIndex.h
│   └── SearchServer.h
├── src/
│   ├── ConverterJSON.cpp
│   ├── InvertedIndex.cpp
│   ├── main.cpp
│   └── SearchServer.cpp
└── resources/
├── file001.txt
├── file002.txt
├── file003.txt
└── file004.txt
</div>

<div align="center">
Диаграмма классов
  <img src="https://via.placeholder.com/800x400?text=Class+Diagram" alt="Class Diagram"/>
</div>

<div align="center">
🔍 Алгоритм поиска

Индексация:

Многопоточная обработка документов
Создание частотного словаря (инвертированного индекса)
Хранение информации о вхождении каждого слова в документах

Поиск:

При поиске сначала обрабатываются самые редкие слова запроса
Релевантность документа определяется суммой частот всех слов запроса
Относительная релевантность рассчитывается как отношение к максимальному значению

Ранжирование:

Документы сортируются по убыванию релевантности
При равной релевантности сортируются по возрастанию идентификатора документа
Количество результатов ограничивается параметром max_responses
</div>

<div align="center">
🔧 Технические детали
Многопоточность
Индексация документов выполняется параллельно, что увеличивает производительность системы, особенно при большом количестве документов.

cpp

void InvertedIndex::UpdateDocumentBase(std::vector<std::string> input_docs) {
// ...
std::vector<std::thread> indexing_threads;
for (size_t doc_id = 0; doc_id < docs.size(); ++doc_id) {
indexing_threads.emplace_back(&InvertedIndex::IndexDocument, this, doc_id, std::ref(docs[doc_id]));
}
for (auto& thread : indexing_threads) {
if (thread.joinable()) {
thread.join();
}
}
// ...
}
</div> 
<div align="center">
Структуры данных
Система использует следующие структуры данных:


std::map<std::string, std::vector<Entry>> - частотный словарь слов

struct Entry {size_t doc_id, count} - структура для хранения информации о вхождении слова в документ

struct RelativeIndex {size_t doc_id, float rank} - структура для хранения результатов поиска
</div>



Проект разрабатывался в учебных целях.
Ссылка на проект: https://github.com/xdsasdsd/study-project

