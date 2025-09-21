#include <iostream>
#include <vector>
#include <ctime>
#include <sstream>

using namespace std;


class file;
long long compute_hash(const std::string& s);
file* getFile(string filename);
vector<string> file_heap1;
vector<string> file_heap2;

class TreeNode{
    private:
        time_t snapshot_timestamp;
    public:
        string message="";
        time_t created_timestamp;
        TreeNode* parent;
        string content;
        int version_id;
        vector<TreeNode*> children;
        TreeNode(TreeNode* father){
            created_timestamp = time(NULL);
            parent=father;
        }
        ~TreeNode(){
            for (TreeNode* child : children) {
                delete child;
            }
        }
        void makeSnapshot(string msg){
            message = msg;
            snapshot_timestamp = time(NULL);
        }
        bool isSnapshot(){
            return message != "";
        }
        void update(string cont){
            content = cont;
        }
};

class file{
    private:
        TreeNode* root;
        TreeNode* active_version;
        vector<TreeNode*> version_track;
        // map<int, TreeNode> version_map;
    public:
        int total_versions;
        int pos_hash1;
        int pos_hash2;
        time_t modified_time;
        file(){}
        ~file(){
            delete root;
        }
        void create(string msg){
            modified_time = time(NULL);
            TreeNode* temp = new TreeNode(nullptr);
            root=temp;
            root->version_id=0;
            root->makeSnapshot(msg);
            active_version = root;
            total_versions=1;
            version_track.push_back(root);
        }
        string read(){
            return active_version->content;
        }
        void insert(string cont, string filename){
            modified_time = time(NULL);
            correct_heap(filename);
            if(active_version->isSnapshot()){
                TreeNode* temp2 = new TreeNode(active_version);
                temp2->parent = active_version;
                active_version->children.push_back(temp2);
                active_version = temp2;
                total_versions+=1;
                active_version->version_id = version_track.size();
                version_track.push_back(active_version);
                active_version->content = cont;
            }
            else{
                active_version->content = active_version->content+cont;
            }
        }
        void update(string cont, string filename){
            modified_time = time(NULL);
            correct_heap(filename);
            if(active_version->isSnapshot()){
                TreeNode* temp2 = new TreeNode(active_version);
                temp2->parent = active_version;
                active_version->children.push_back(temp2);
                active_version = temp2;
                total_versions+=1;
                active_version->version_id = version_track.size();
                version_track.push_back(active_version);
                active_version->content = cont;
            }
            else{
                active_version->content = cont;
            }
        }
        void snapshot(string msg){
            active_version->makeSnapshot(msg);
        }
        void rollback(int version_id=-1){
            if(version_id==-1){
                if(active_version->parent==nullptr){
                    std::cerr << "No Parent exists" << '\n';
                }
                else{
                    active_version = active_version->parent;
                }
            }
            else{
                if(version_id<0 || version_id>=version_track.size()){
                    std::cerr << "version id does not exist" << '\n'; 
                }
                else{
                    active_version = version_track[version_id];
                }
            }
        }
        void history(){
            TreeNode* curr = active_version;
            vector<TreeNode*> h;
            while(curr!=nullptr){
                if(curr->isSnapshot()){
                    h.push_back(curr);
                }
                curr=curr->parent;
            }
            for(int i=0; i<h.size(); i++){
                cout << "ID:"<< h[h.size()-i-1]->version_id << ". Timestamp: " << h[h.size()-i-1]->created_timestamp << ". Message: " << h[h.size()-i-1]->message << '\n';
            }
        }
        void correct_heap(string filename){
            while (pos_hash1 > 0 && getFile(file_heap1[(pos_hash1 - 1) / 2])->modified_time < getFile(file_heap1[pos_hash1])->modified_time) {
                int parent_index = (pos_hash1 - 1) / 2;
                int child_index = pos_hash1;
                file* parent_file = getFile(file_heap1[parent_index]);
                file* child_file = getFile(file_heap1[child_index]);

                string temp = file_heap1[parent_index];
                file_heap1[parent_index] = file_heap1[child_index];
                file_heap1[child_index] = temp;

                parent_file->pos_hash1 = child_index;
                child_file->pos_hash1 = parent_index;

                pos_hash1 = parent_index;
            }

            while (pos_hash2 > 0 && getFile(file_heap2[(pos_hash2 - 1) / 2])->total_versions < getFile(file_heap2[pos_hash2])->total_versions) {
                int parent_index = (pos_hash2 - 1) / 2;
                int child_index = pos_hash2;
                file* parent_file = getFile(file_heap2[parent_index]);
                file* child_file = getFile(file_heap2[child_index]);

                string temp = file_heap2[parent_index];
                file_heap2[parent_index] = file_heap2[child_index];
                file_heap2[child_index] = temp;

                parent_file->pos_hash2 = child_index;
                child_file->pos_hash2 = parent_index;

                pos_hash2 = parent_index;
            }
        }
};

class Node{
    public:
        file* data;
        string filename;
        Node* next;
        Node(file* val, string name){
            data=val;
            filename=name;
            next=nullptr;
        }
};

class LinkedList{
    public:
        Node* head;
        Node* tail;
        LinkedList(){
            head=nullptr;
            tail=nullptr;
        }
        ~LinkedList(){
            Node* current = head;
            Node* nextNode = nullptr;
            while (current != nullptr){
                nextNode = current->next;
                delete current;          
                current = nextNode;      
            }
        }
        void push(file* val, string name){
            Node* temp = new Node(val, name);
            if(head==nullptr){
                head=temp;
                tail=temp;
            }
            else{
                tail->next = temp;
                tail=temp;
            }
        }
        file* find(string name){
            Node* curr = head;
            while(curr!=nullptr){
                if(curr->filename==name){
                    return curr->data;
                }
                curr=curr->next;
            }
            return nullptr;
        }
};

LinkedList f[199];

file* getFile(string filename){
    int hash_value = compute_hash(filename)%199;
    file* foundfile = f[hash_value].find(filename);
    return foundfile;
}

long long compute_hash(const std::string& s) {
    const int p = 31;
    const int m = 1e9 + 9;
    long long hash_value = 0;
    long long p_pow = 1;
    for (char c : s) {
        hash_value = (hash_value + c * p_pow) % m;
        p_pow = (p_pow * p) % m;
    }
    return (hash_value % 199 + 199) % 199;
}

class fileSystem{

    public:
        fileSystem(){}
        ~fileSystem() {
            for (int i = 0; i < 199; i++) {
                Node* current = f[i].head;
                while (current != nullptr) {
                    delete current->data;
                    current = current->next;
                }
            }
        }
        void add_file(file* newfile, string filename){
            int hash_value = compute_hash(filename)%199;
            f[hash_value].push(newfile, filename);
            newfile->pos_hash1 = insert1(file_heap1, filename);
            newfile->pos_hash2 = insert2(file_heap2, filename);
        }
        int insert1(vector<string>& h, string val){
            h.push_back(val);
            int n = h.size()-1;
            while (n > 0 && getFile(h[(n - 1) / 2])->modified_time < getFile(h[n])->modified_time) {
                int parent_index = (n - 1) / 2;
                int child_index = n;

                file* parent_file = getFile(h[parent_index]);
                file* child_file = getFile(h[child_index]);

                string temp = h[parent_index];
                h[parent_index] = h[child_index];
                h[child_index] = temp;

                parent_file->pos_hash1 = child_index; 
                child_file->pos_hash1 = parent_index; 

                n = parent_index;
            }
            return n;
        }
        string deleteRoot1(vector<string>& h){

            if(h.empty()){
                return "";
            }
            int n=h.size()-1;
            string ans=h[0];
            h[0]=h[n];
            getFile(h[0])->pos_hash1 = 0;
            h.pop_back();
            n--;
            int i=0;
            while(true){
                int left_child = 2*i+1;
                int right_child = 2*i+2;
                int max=i;
                if(left_child<=n && getFile(h[left_child])->modified_time>=getFile(h[max])->modified_time){
                    max=left_child;
                }
                if(right_child<=n && getFile(h[right_child])->modified_time>=getFile(h[max])->modified_time){
                    max=right_child;
                }
                if(i!=max){
                    getFile(h[max])->pos_hash1=i;
                    getFile(h[i])->pos_hash1=max;
                    string temp = h[i];
                    h[i] = h[max];
                    h[max] = temp;
                    i = max;
                }
                else{
                    break;
                }
            }
            return ans;
        }
        void recentFiles(int num){
            vector<string> deleted_files;
            int number_of_files = file_heap1.size();
            if(num>number_of_files){
                cerr << "num is greater than the number of files in the system" << '\n';
            }
            else{
                for(int i=0; i<num; i++){
                    deleted_files.push_back(file_heap1[0]);
                    cout << deleteRoot1(file_heap1) << " ";
                }
                for(int i=0; i<num; i++){
                    insert1(file_heap1, deleted_files[i]);
                }
                cout << '\n';
            }
            

        }
        int insert2(vector<string>& h, string val){
            h.push_back(val);
            int n = h.size()-1;
            while (n > 0 && getFile(h[(n - 1) / 2])->total_versions < getFile(h[n])->total_versions) {
                int parent_index = (n - 1) / 2;
                int child_index = n;

                file* parent_file = getFile(h[parent_index]);
                file* child_file = getFile(h[child_index]);

                string temp = h[parent_index];
                h[parent_index] = h[child_index];
                h[child_index] = temp;

                parent_file->pos_hash2 = child_index; 
                child_file->pos_hash2 = parent_index; 

                n = parent_index;
            }
            return n;
        }
        string deleteRoot2(vector<string>& h){

            if(h.empty()){
                return "";
            }
            int n=h.size()-1;
            string ans=h[0];
            h[0]=h[n];
            getFile(h[0])->pos_hash2 = 0;
            h.pop_back();
            n--;
            int i=0;
            while(true){
                int left_child = 2*i+1;
                int right_child = 2*i+2;
                int max=i;
                if(left_child<=n && getFile(h[left_child])->total_versions>=getFile(h[max])->total_versions){
                    max=left_child;
                }
                if(right_child<=n && getFile(h[right_child])->total_versions>=getFile(h[max])->total_versions){
                    max=right_child;
                }
                if(i!=max){
                    getFile(h[max])->pos_hash2=i;
                    getFile(h[i])->pos_hash2=max;
                    string temp = h[i];
                    h[i] = h[max];
                    h[max] = temp;
                    i = max;
                }
                else{
                    break;
                }
            }
            return ans;
        }
        void biggestTrees(int num){
            vector<string> deleted_files;
            int number_of_files = file_heap2.size();
            if(num>number_of_files){
                cerr << "num is greater than the number of files in the system" << '\n';
            }
            else{
                for(int i=0; i<num; i++){
                    deleted_files.push_back(file_heap2[0]);
                    cout << deleteRoot2(file_heap2) << " ";
                }
                for(int i=0; i<num; i++){
                    insert2(file_heap2, deleted_files[i]);
                }
                cout << '\n';
            }
        }
};

int main(){
    fileSystem fs;
    while(true){
        cout << "> ";
        string line;
        getline(cin, line);
        if(line=="EXIT"){
            break;
        }
        stringstream ss(line);
        string word;
        vector<string> l;
        while(ss>>word){
            l.push_back(word);
        }

        if(!l.empty()){
            if(l[0]=="CREATE"){
                if(l.size()!=2){
                    cerr << "INCORRECT COMMAND" << '\n';
                }
                else{
                    string name = l[1];
                    if(getFile(l[1])!=nullptr){
                        cerr << "File already exits" << '\n';
                    }
                    else{
                        file* temp = new file();
                        temp->create("Default Message");
                        fs.add_file(temp, name);
                    }
                }
            }
            else if(l[0]=="READ"){
                if(l.size()!=2){
                    cerr << "INCORRECT COMMAND" << '\n';
                }
                else{
                    string name = l[1];
                    file* current_file;
                    current_file =  getFile(name);
                    if(current_file==nullptr){
                        cerr << "File doesnt exist" << '\n';
                    }
                    else{
                        cout << current_file->read() << '\n';
                    }
                }
            }
            else if(l[0]=="INSERT"){
                if(l.size()==1){
                    cerr << "INCORRECT COMMAND" << '\n';
                }
                else{
                    string name = l[1];
                    file* current_file;
                    current_file =  getFile(name);
                    if(current_file==nullptr){
                        cerr << "File doesnt exist" << '\n';
                    }
                    else if(l.size()==2){
                        cerr << "Please Provide Content" << '\n';
                    }
                    else{
                        string content="";
                        for(int i=2; i<l.size(); i++){
                            content += l[i];
                            if(i!=l.size()-1){
                                content += " ";
                            }
                        }
                        current_file->insert(content, name);
                    }
                }
            }
            else if(l[0]=="UPDATE"){
                if(l.size()==1){
                    cerr << "INCORRECT COMMAND" << '\n';
                }
                else{
                    string name = l[1];
                    file* current_file;
                    current_file =  getFile(name);
                    if(current_file==nullptr){
                        cerr << "File doesnt exist" << '\n';
                    }
                    else if(l.size()==2){
                        cerr << "Please Provide Content" << '\n';
                    }
                    else{
                        string content="";
                        for(int i=2; i<l.size(); i++){
                            content += l[i];
                            if(i!=l.size()-1){
                                content += " ";
                            }
                        }
                        current_file->update(content, name);
                    }
                }
            }
            else if(l[0]=="SNAPSHOT"){
                if(l.size()==1){
                    cerr << "INCORRECT COMMAND" << '\n';
                }
                else{
                    string name = l[1];
                    file* current_file;
                    current_file =  getFile(name);
                    if(current_file==nullptr){
                        cerr << "File doesnt exist" << '\n';
                    }
                    else if(l.size()==2){
                        cerr << "Please Provide Message" << '\n';
                    }
                    else{
                        string message="";
                        for(int i=2; i<l.size(); i++){
                            message += l[i];
                            if(i!=l.size()-1){
                                message += " ";
                            }
                        }
                        current_file->snapshot(message);
                    }
                }
            }
            else if(l[0]=="ROLLBACK"){
                if(l.size()==1){
                    cerr << "INCORRECT COMMAND" << '\n';
                }
                else{
                    bool run=true;
                    string name = l[1];
                    file* current_file;
                    current_file =  getFile(name);
                    if(current_file==nullptr){
                        cerr << "File doesnt exist" << '\n';
                    }
                    else{
                        if(l.size()==2){
                            current_file->rollback();
                        }
                        else{
                            int id=-1;
                            try{
                                id = stoi(l[2]);
                                if(id<0){
                                    run=false;
                                    cerr << "Enter a valid version id" << '\n';
                                }
                            }
                            catch(const invalid_argument& e){
                                run=false;
                                cerr << "Error: Version ID must be a valid integer" << '\n';
                            }
                            catch(const out_of_range& e){
                                run=false;
                                cerr << "Error: Version ID is out of range" << '\n';

                            }
                            if(run){
                                current_file->rollback(id);
                            }
                        }
                    }
                }
            }
            else if(l[0]=="HISTORY"){
                if(l.size()!=2){
                    cerr << "INCORRECT COMMAND" << '\n';
                }
                else{
                    string name = l[1];
                    file* current_file;
                    current_file =  getFile(name);
                    if(current_file==nullptr){
                        cerr << "File doesnt exist" << '\n';
                    }
                    else{
                        current_file->history();
                    }
                }
            }
            else if(l[0]=="RECENT_FILES"){
                if(l.size()!=2){
                    cerr << "INCORRECT COMMAND" << '\n';
                }
                else{
                    bool run=true;
                    int num;
                    try{
                        num = stoi(l[1]);
                        if(num<=0){
                            run=false;
                            cerr << "Enter a valid version id" << '\n';
                        }
                    }
                    catch(const invalid_argument& e){
                        run=false;
                        cerr << "Error: Version ID must be a valid integer" << '\n';
                    }
                    catch(const out_of_range& e){
                        run=false;
                        cerr << "Error: Version ID is out of range" << '\n';

                    }
                    if(run){
                        fs.recentFiles(num);
                    }
                }
            }
            else if(l[0]=="BIGGEST_TREES"){
                if(l.size()!=2){
                    cerr << "INCORRECT COMMAND" << '\n';
                }
                else{
                    bool run=true;
                    int num;
                    try{
                        num = stoi(l[1]);
                        if(num<=0){
                            run=false;
                            cerr << "Enter a valid version id" << '\n';
                        }
                    }
                    catch(const invalid_argument& e){
                        run=false;
                        cerr << "Error: Version ID must be a valid integer" << '\n';
                    }
                    catch(const out_of_range& e){
                        run=false;
                        cerr << "Error: Version ID is out of range" << '\n';

                    }
                    if(run){
                        fs.biggestTrees(num);
                    }
                }
            }
            else{
                cerr << "UNKNOWN COMMAND" << '\n';
            }
        }
        else{
            cerr << "ENTER A COMMAND PLEASE" << '\n';
        }
    }

    return 0;
}