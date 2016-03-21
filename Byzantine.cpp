/*
 * Author: Praveen Kumar
 * BITS ID: 2015HT12227
 */

#include <iostream>
#include <map>
#include <vector>
#include <string>
#include <sstream>

//
// Some useful global definitions
//
typedef std::string Path;
const char ONE = '1';
const char ZERO = '0';
const char UNKNOWN = '?';
const char FAULTY = 'X';
const char DEFAULT = ONE;

int N = 3;
int M = 1;
int SOURCE = 1;
const bool isDebug = true;

//
// Each process has a map of nodes, with the index to the map being
// a path -  a string that consists of the list of process IDs that
// the information came through. The input value of the node is set when
// the message is received in the first round. The output value of the
// node is set in the second round.
//
// Note that we don't ever use the default constructor, but it is
// required that we have one if we are going to store these objects
// in a map
//
struct Node {
    Node( char input = FAULTY, char output = FAULTY )
    : input_value( input )
    , output_value( output )
    {};
    char input_value;
    char output_value;
};


//
// The Traits base class is used to define the messaging behavior for
// the processes. (Traits might not be the best name, as I am overloading it
// a bit from its common use, but it's close.) The Process class has a global
// object that of type Traits that it uses to get all the information used
// to characterize the information about who is a general, who is a lieutentant,
// and so on.
//
// The Process class has a static Traits member whose methods are called
// at key points in the messaging and decision phases to determine how
// to behave. You change the configuration of the run by modifying this Tratis
// class
//

class Traits {
    public :
    Traits( int source, int m, int n, bool isDebug = false )
    : mSource( source )
    , mM( m )
    , mN( n )
    , misDebug( isDebug )
    {}

    //
    // This method returns the true value of the source's value. The source may send
    // faulty values to other processes, but the Node returned by this value will be
    // in its root node.
    //
    Node GetSourceValue(bool isFaulty) {
        if(isFaulty) {
           return Node( UNKNOWN, UNKNOWN );
        }else {
            return Node( ONE, UNKNOWN );
        }
        
    }
    //
    // During message, GetValue() is called to get the value returned by a given process
    // during a messaging round. 'value' is the input value that it should be sending to
    // the destination process (if it isn't faulty), source is the source process ID,
    // destination is the destination process ID, and Path is the path being used for this
    // particular message.
    //
    char GetValue( char value, int source, int destination, const Path &path, const bool faulty )
    {
        if(faulty) {
            return UNKNOWN;
        }else {
            return value;
        }
    }
    //
    // When breaking a tie, GetDefault() is used to get the default value.
    //
    // This is an arbitrary decision, but it has to be consistent across all processes.
    // More importantly, the processes have to arrive at a correct decision regardless
    // of whether the default value is always 0 or always 1. In this case we've set it to
    // a value of 1.
    //
    char GetDefault()
    {
        return DEFAULT;
    }
    
    //
    // This member holds the process id for the source process. There are a few
    // times in the course of the messaging and decision making process when we
    // have to know if a process is the source process. (Note that we could
    // have a GetSource() member, but since this is a const, why not just
    // expose it.)
    //
    int mSource;
    //
    // These two members hold M and N, the number of rounds of messaging and
    // the number of processes. Again, these could be exposed by methods, but
    // since I can make them const, why expose them directly as publics.
    //
    int mM;
    size_t mN;
    //
    // Turn this member on by passing true in the constructor, and you will get
    // some addition trace output
    //
    const bool misDebug;
};


class Process {
    bool faulty;
    public :
    //
    // The Process constructor only has a couple of interesting thigns to do.
    // First, if it finds that the static mChildren map is empty, it calls
    // GenerateChildren(), a static method which generats the map used throughout
    // the program to diagram the tree which holds the topology of the message
    // tree. See GenerateChildren() below for details.
    //
    // The second thing of note is that if this is the source process (the General)
    // we initialize the default path with the General's source value - a node
    // that will contain the General's proposed value and nothing else.
    //
    Process( int id )
    : mId( id )
    {
        if ( mChildren.size() == 0 )
            GenerateChildren( mTraits.mM, mTraits.mN, std::vector<bool>( mTraits.mN, true ) );
        if ( mId == mTraits.mSource )
            mNodes[ "" ] = mTraits.GetSourceValue(faulty);
    }
    
    static void updateTraits() {
        mTraits.mSource = SOURCE;
        mTraits.mN = N;
        mTraits.mM = M;
    }
    
    void SetFaulty(bool isFaulty){
        faulty = isFaulty;
    }
    
    //
    // After constructing all messages, you need to call SendMessages on each process,
    // once per round. This routine will send the appropriate messages for each round
    // to all th eother processes listed in the vector passed in as an argument.
    //
    // Deciding on what messages to send is pretty simple. If we look at the static
    // map mPathsByRank, indexed by round and the processId of this process, it gives
    // the entire set of taraget paths that this process needs to send messages to.
    // So there is an iteration loop through that map, and this process sends a message
    // to the correct target process for each path in the map.
    //
    // Note that we go to the Traits class to actually get the node value we send in the
    // message - this allows for faulty processes to send specifically tailored deceptive
    // messages.
    //
    // Also, if the isDebug flag is turned on, information about the message is printed to the
    // console.
    //
    void SendMessages( int round, std::vector<Process> &processes )
    {
        for ( size_t i = 0 ; i < mPathsByRank[ round ][ mId ].size() ; i++ )
        {
            Path source_node_path = mPathsByRank[ round ][ mId ][ i ];
            source_node_path = source_node_path.substr( 0, source_node_path.size() - 1 );
            Node source_node = mNodes[ source_node_path ];
            for ( size_t j = 0 ; j < mTraits.mN ; j++ )
                if ( j != mTraits.mSource ) {
                    char value = mTraits.GetValue( source_node.input_value,
                                                  mId,
                                                  (int) j,
                                                  mPathsByRank[ round ][ mId ][ i ],processes[j].IsFaulty() );
                    if ( mTraits.misDebug )
                        std::cout << "Sending from process " << mId
                        << " to " << static_cast<unsigned int>( j )
                        << ": {" << value << ", "
                        << mPathsByRank[ round ][ mId ][ i ]
                        << ", " << UNKNOWN << "}"
                        << "\n";
                    processes[ j ].ReceiveMessage( mPathsByRank[ round ][ mId ][ i ],
                                                  Node( value, UNKNOWN ) );
                }
            std::cout<<"\n";
        }
    }
    
    //
    // After all messages have been sent, it's time to Decide.
    //
    // This part of the algorithm follows the description in the article closely.
    // It has to work its way from the leaf values up to the root of the tree.
    // The first round consists of going to the leaf nodes, and copying the input
    // value to the output value.
    //
    // All subsequent rounds consist of getting the majority of the output values from
    // each nodes children, and copying that to the nodes output value.
    //
    // When we finally reach the root node, there is only one node with an output value,
    // and that represents this processes decision.
    //
    char Decide()
    {
        //
        // The source process doesn't have to do all the work - since it's the decider,
        // it simply looks at its input value to pick the appropriate decision value.
        //
        if ( mId == mTraits.mSource )
            return mNodes[ "" ].input_value;
        //
        // Step 1 - set the leaf values
        //
        for ( size_t i = 0 ; i < mTraits.mN ; i++ )
            for ( size_t j = 0 ; j < mPathsByRank[ mTraits.mM ][ i ].size() ; j++ )
            {
                const Path &path = mPathsByRank[ mTraits.mM ][ i ][ j ];
                Node &node = mNodes[ path ];
                node.output_value = node.input_value;
            }
        //
        // Step 2 - work up the tree
        //
        for ( int round = (int) mTraits.mM - 1 ; round >= 0 ; round-- )
        {
            for ( size_t i = 0 ; i < mTraits.mN ; i++ )
                for ( size_t j = 0 ; j < mPathsByRank[ round ][ i ].size() ; j++ )
                {
                    const Path &path = mPathsByRank[ round ][ i ][ j ];
                    Node &node = mNodes[ path ];
                    node.output_value = GetMajority( path );
                }
        }
        const Path &top_path = mPathsByRank[ 0 ][ mTraits.mSource ].front();
        const Node &top_node = mNodes[ top_path ];
        return top_node.output_value;
    }
    
    //
    // This isDebug routine is used to dump out the contents of the tree in text form.
    // It's not too hard to read if the number of processes is not too big.
    //
    // It operates recursively, going through the tree and dumping the children of each
    // node before dumping the node itself.
    //
    std::string Dump( Path path = "" )
    {
        if ( path == "" )
            path = mPathsByRank[ 0 ][ mTraits.mSource ].front();
        std::stringstream s;
        for ( size_t i = 0 ; i < mChildren[ path ].size() ; i++ )
            s << Dump( mChildren[ path ][ i ] );
        Node &node = mNodes[ path ];
        s << "{" << node.input_value
        << "," << path
        << "," << node.output_value
        << "}\n";
        return s.str();
    }
    
    //
    // A utility routine that tells whether a given process is faulty
    //
    bool IsFaulty()
    {
        return faulty;
    }
    
    //
    // Another somewhat handy utility routine
    //
    bool IsSource()
    {
        return mTraits.mSource == mId;
    }
    
    private :
    int mId;                    //The integer ID of the process
    std::map<Path,Node> mNodes; //The map that holds the process tree
   
    //
    // Static data shared among all process objects
    //
    static Traits mTraits;
    static std::map<Path, std::vector<Path> > mChildren;
    static std::map<size_t, std::map<size_t, std::vector<Path> > > mPathsByRank;
   
    //
    // This routine calculates the majority value for the children of a given
    // path. The logic is pretty simple, we increment the count for all possible
    // values over the children. If there is a clearcut majority, we return that,
    // otherwise we return the default value defined by the Traits class.
    //
    char GetMajority( const Path &path )
    {
        std::map<char,size_t> counts;
        counts[ ONE ] = 0;
        counts[ ZERO ] = 0;
        counts[ UNKNOWN ] = 0;
        size_t n = mChildren[ path ].size();
        for ( size_t i = 0 ; i < n ; i++ ) {
            const Path &child = mChildren[ path ][ i ];
            const Node &node = mNodes[ child ];
            counts[ node.output_value ]++;
        }
        if ( counts[ ONE ] > ( n / 2 ) )
            return ONE;
        if ( counts[ ZERO ] > ( n / 2 ) )
            return ZERO;
        if ( counts[ ONE ] == counts[ ZERO ] && counts[ ONE ] == ( n /2 ) )
            return mTraits.GetDefault();
        return UNKNOWN;
    }
   
    //
    // Receiving a message is pretty simple here, it means that some other process
    // calls this method on the current process with path and a node. All we do
    // is store the value, we'll use it in the next round of messaging.
    //
    void ReceiveMessage( const Path &path, const Node &node )
    {
        mNodes[ path ] = node;
    }
   
    //
    // This static method is called by the first process that is constructed. It generates
    // a static copy of the tree in the mChildren static map. There are many times
    // in the operation of the algorithm that we have a path and want to find all the
    // children for that node. By looking up mChildren[ path ] we get a vector
    // containing all children for that path.
    //
    // This recursive routine has some isDebug output that is useful in isDebugging. If
    // traits object has the isDebug member set, it will be printed out
    //
    static void GenerateChildren( const size_t m,
                                 const size_t n,
                                 std::vector<bool> ids,
                                 int source = mTraits.mSource,
                                 Path current_path = "",
                                 size_t rank = 0 )
    {
        ids[ source ] = false;
        current_path += static_cast<char>( source + '0' );
        mPathsByRank[ rank ][ source ].push_back( current_path );
        if ( rank < m )
            for ( int i = 0 ; i < static_cast<int>( ids.size() ) ; i++ )
                if ( ids[ i ] ) {
                    GenerateChildren( m, n, ids, i, current_path, rank + 1 );
                    mChildren[ current_path ].push_back( current_path + static_cast<char>( i + '0' ) );
                }
        if ( mTraits.misDebug ) {
            size_t j = 0;
            if(j<mChildren[current_path].size()) {
                std::cout <<"\nSource = "<<current_path<<"\nChildren = ";
            }
            for ( size_t j = 0 ; j < mChildren[current_path].size() ; j++ ) {
                std::cout <<mChildren[current_path][ j ];
                if(j+1 < mChildren[current_path].size()) {
                    std::cout<<",";
                }else {
                    std::cout<<"\n";
                }
            }
        }
    }
};

//
// The definition of the three static members used by the Process class
//

std::map<Path, std::vector<Path> > Process::mChildren;
std::map<size_t, std::map<size_t, std::vector<Path> > >  Process::mPathsByRank;
Traits Process::mTraits = Traits( SOURCE, M, N, isDebug );

int main()
{
    std::cout<<"Enter number of Generals: ";
    std::cin>>N;
    
    std::cout<<"Enter number of Traitors: ";
    std::cin>>M;
    
    srand (time(NULL));
    SOURCE = rand() % N;
    
    Process::updateTraits();
    
    //
    // Create the message tree
    //
    std::vector<Process> processes;
    for ( int i = 0 ; i < N ; i++ )
        processes.push_back( Process( i ) );
    
    int i = 0;
    while(i < M) {
        int randnum = rand() % N;
        if(randnum != SOURCE) {
            processes[randnum].SetFaulty(true);
            i++;
        }
    }
    
    
    std::cout<<"================================== Messages =====================================\n";
    
    //
    // Starting at round 0 and working up to round M, call the
    // SendMessages() method of each process. It will send the appropriate
    // message to all other sibling processes.
    //
    for ( int i = 0 ; i <= M ; i++ ){
        for ( int j = 0 ; j < N ; j++ ) {
            processes[ j ].SendMessages( i, processes );
        }
    }
    //
    // All that is left is to print out the results. For non-faulty processes,
    // we call the Decide() method to see what what the process decision was
    //
    std::map<char,size_t> counts;
    counts[ ONE ] = 0;
    counts[ ZERO ] = 0;
    counts[ UNKNOWN ] = 0;
    
    for ( int j = 0 ; j < N ; j++ ) {
        
        if ( processes[ j ].IsSource() )
            std::cout << "Source ";
        std::cout << "Process " << j;
        if ( !processes[ j ].IsFaulty() ){
            
            char output = processes[ j ].Decide();
            counts[ output ]++;
            std::cout << " decides on value " <<output;
            
        }
        else{
            std::cout << " is faulty";
        }
        std::cout<<"\n===================================\n";
        
        std::cout<< processes[ i ].Dump() << "\n";
    }
    
    
    std::cout<<"================================== Consensus =====================================\n";
    
    // Compute consensus
    int loyalCount = N - M;
    if ( counts[ ONE ] > ( loyalCount / 2 ) )
        std::cout<<"\nCONSENSUS REACHED!\n";
    else if ( counts[ ZERO ] > loyalCount / 2 )
        std::cout<<"\nCONSENSUS REACHED!\n";
    else if ( counts[ ONE ] == counts[ ZERO ] && counts[ ONE ] == ( loyalCount / 2 ) )
        std::cout<<"\nTIE - BREAKING USING DEFAULT VALUE\n";
    else
        std::cout<<"\nUNABLE TO REACH CONSENSUS!\n";
    
    std::cout << "\n";
    
    return 0;
}
