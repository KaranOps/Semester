// #include<stdio.h>
// #include<stdlib.h>

// int partition(double arr[], int start, int end){
//     double pivot = arr[end];
//     int i = start-1;

//     for(int j=0;j<end;j++){
//         if(arr[j]<pivot){
//             i++;

//             double temp = arr[i];
//             arr[i]=arr[j];
//             arr[j] = temp;
//         }
//         i++;
//         int temp = arr[i];
//         arr[i] = arr[j];
//         arr[j] = temp;
//         return j;
//     }
// }

// void quickSort(double arr[], int start, int end){
//     if(start < end){
//         int pIdx = partition(arr,start,end);

//         quickSort(arr,start,pIdx-1);
//         quickSort(arr,pIdx,end);
//     }
// }

// int main(){

//     return 0;
// }

long long widthOfBinaryTree(TreeNode* root) {
        if(root==NULL){
            return 0;
        }
            long long res=0;
            
            //Making queue for level order traversal "int" store size of queue
            queue<pair<TreeNode* , int> > q;
            q.push({root, 0});
            //Now level order traversal
            while(!q.empty()){
            long long leftMost,rightMost;
                int size = q.size();
                long long currMin = q.front().second;
                for(int i=0;i<size;i++){
                    long long currId = q.front().second - currMin; 
                    //This makes the index of leftMost node in any level as zero and rightMost as last value
                    //To rid of the integer overflow
                    TreeNode* temp = q.front().first;
                    q.pop();
                    
                    if(i==0) leftMost = currId;
                    if(i==size-1) rightMost = currId;

                    if(root->left){
                        q.push({root->left, currId*2+1});
                    }
                    if(root->right){
                        q.push({root->right, currId*2+2});
                    }
                }
                //Doing max becoz it may be possible that in the last 
                // level there is less node compare to 2nd last or upper level
                res = max(res, rightMost-leftMost+1); 
            }
        return res;
    }