enum Kind{
    undefined = -1,
    boolean = 0,
    integer = 1,
    string = 2,
    enumerator = 3,
    question_answer = 4,
};

class Range{
    public:
    Range(int s, int e);
    Range();
    int start;
    int end;
    bool operator==(const Range &r) const;
};


