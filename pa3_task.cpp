#include <cstring>

#include "constants.h"
#include "given.h"
#include "pa3_task.h"
#include "structures.h"

using namespace std;

/**
 * Task 1 - Register New User
 *
 * The `register_new_user` function registers a new user on the version control
 * system if no existing user has the specified username already.
 *
 * @param userManagement: reference to the `UserManagement` structure containing
 *                        the linked list of users.
 * @param username: the username of the new user to create.
 * @returns: `nullptr` if any validation failed; a pointer to the newly created
 *           user otherwise.
 */
const User *register_new_user(UserManagement &userManagement,
                              const char *username) {
  // TODO: Task 1
  
  //if the specified username is too long
  if (strlen(username) >= MAX_USER_NAME_LEN) 
  {
    cout << "Username length exceeded " << (MAX_USER_NAME_LEN - 1) << " characters." << endl;
    return nullptr;
  }

  //if the specified username is already associated to a user on the system
  User *curr = userManagement.head;

  while (curr != nullptr) //traverse the linked list of users
  { 
    if (strcmp(curr->name, username) == 0)  //check if the current user has the same username as the specified username
    {  
      cout << "User " << username << " already exists." << endl;
      return nullptr;
    }
    curr = curr->next;
  }

  // If we reach here, the username is available

  User *newUser = new User; //create pointer to the new user
  strcpy(newUser->name, username); //copy the specified username to the name field of the new user
  newUser->numRepos = 0; //initialize numRepos to 0
  newUser->repos = nullptr; //initialize repos to nullptr

  //add the new user to the linked list of users, (sorted in lexicographical order)
  if (userManagement.head == nullptr || strcmp(userManagement.head->name, username) > 0) //username is lexicographically smaller than the head of the linked list of users, or the linked list is empty
  {  
    newUser->next = userManagement.head;
    userManagement.head = newUser;
  } 
  else //username is lexicographically greater than the head of the linked list of users
  {
    User *prev = userManagement.head;
    while (prev->next != nullptr && strcmp(prev->next->name, username) < 0) //traverse the linked list of users 
    { // until we find a user with a username that is lexicographically greater than the specified username, or we reach the end of the linked list
      prev = prev->next;
    }
    if (prev->next == nullptr || strcmp(prev->next->name, username) > 0) //if we reach the end of the linked list, or the next user has a username that is lexicographically greater than the specified username
    {
      newUser->next = prev->next; //if next user is lexicographically greater than the username, insert the new user before the next user
      prev->next = newUser;
    }
  }

  return newUser;
}

/**
 * Task 2 - Create Repository
 *
 * The `create_repository` function creates a repository with the specified name
 * under the specified owner, if no existing repository under the very user has
 * the same name as specified.
 *
 * @param repoManagement: reference to the `RepositoryManagement` structure
 *                        containing the dynamic array of pointers to
 *                        repositories.
 * @param owner: the owner of the new repository.
 * @param repoName: the name of the new repository.
 * @param creationTimestamp: the timestamp for the creation of the repository,
 *                           used for the initial commit.
 * @returns: -1 if any validation fails before repository creation; the index
 *           into the array of pointers to repositories under the owner user
 *           associated with this new repository otherwise. [**Apparently WRONG, according to PIAZZA]
 *          UPDATED: Returns the index of the newly created repository in the dynamic array of repositories 
 *                    in the repository management structure otherwise
 * 
 *            
 */
int create_repository(RepositoryManagement &repoManagement, User *owner,
                      const char *repoName, const time_t creationTimestamp) {
  // TODO: Task 2

  //if the length of the repository name is too long
  if (strlen(repoName) >= MAX_REPO_NAME_LEN) 
  {
    cout << "Repository name length exceeded " << (MAX_REPO_NAME_LEN - 1) << " characters." << endl;
    return -1;
  }

  //if the specified repository to create already exists in the system under the specified owner
  if (owner->repos != nullptr) //if the owner has at least one repository
  {
    for (int i = 0; i < owner->numRepos; i++) //traverse the dynamic array of pointers to repositories under the owner user
    {
      if (strcmp(owner->repos[i]->name, repoName) == 0) //check if the current repository has the same name as the specified repository name
      {
          cout << "Repository " << owner->name << "/" << repoName << " already exists." << endl;
          return -1;
      }
    } 
  }

  // If we reach here, the repository name is available under the owner user

  Repository *newRepo = new Repository; //create a pointer to the new repository
  newRepo->owner = owner; //set the owner field of the new repository to the specified owner
  strcpy(newRepo->name, repoName); //copy the specified repository name to the name field of the new repository
  newRepo->numPrs = 0; //initialize numPrs to 0
  newRepo->prs = nullptr; //initialize prs to nullptr
  newRepo->numForks = 0; //initialize numForks to 0
  newRepo->forks = nullptr; //initialize forks to nullptr
  newRepo->numBranches = 0; //initialize numBranches to 0
  newRepo->branches = nullptr; //initialize branches to nullptr

  //commits set to a pointer to a singleton linked list of commits
  Commit *initialCommit = new Commit; //create a pointer to the initial commit of the new repository
  initialCommit->author = owner; //set the author of the initial commit to the specified owner
  strcpy(initialCommit->message, "Initial commit."); //set the message of the initial commit to "Initial commit"                            
  initialCommit->timestamp = creationTimestamp; //set the timestamp of the initial commit to the specified creation timestamp
  initialCommit->prev = nullptr; //initialize prev to nullptr
  initialCommit->next = nullptr; //initialize next to nullptr

  //compute the hash of initialCommit from the name of author (owner of repository), commit messge and commit timestamp
  SHA1 initialCommit_hasher;
  initialize(initialCommit_hasher);
  input(initialCommit_hasher, initialCommit->author->name, strlen(initialCommit->author->name)); //from name of author
  input(initialCommit_hasher, initialCommit->message, strlen(initialCommit->message)); //from commit message
  input(initialCommit_hasher, initialCommit->timestamp); //from commit timestamp
  digest(initialCommit_hasher); //compute the hash
  initialCommit->hash = initialCommit_hasher; //set the hash of the initial commit to the computed hash

  //initialize commits to point to the initial commit
  newRepo->commits = initialCommit;

  /*
  add the new repository to the dynamic array of repositories in the repository management structure,
  sorted first by the name of the owner, and then the name of the repository, both in lexicographical order.
  */
  if (repoManagement.numRepos == 0) //if there are no repositories in the system
  { 
    //create a new dynamic array of pointers to repositories
    Repository **newReposArray = new Repository*[1];
    newReposArray[0] = newRepo; //add the new repository to the first index of the new dynamic array of pointers to repositories
    repoManagement.repos = newReposArray; //set the dynamic array of pointers to repositories in the repository management structure to the new dynamic array of pointers to repositories
    repoManagement.numRepos = 1; //update the number of repositories in the repository management structure to 1    
  }
  else //find the correct index to insert the new repository in the dynamic array of pointers to repositories, and insert it there
  {
    int insertIndex = 0; //initialize the index to insert the new repository at to 0
    while (insertIndex < repoManagement.numRepos && strcmp(repoManagement.repos[insertIndex]->owner->name, owner->name) < 0) 
    {
      /*
      traverse the dynamic array of pointers to repositories until we find 
      a repository whose owner's name is lexicographically greater than the specified owner's name, 
      or we reach the end of the dynamic array
      */
      insertIndex++;
    }
    while (insertIndex < repoManagement.numRepos && strcmp(repoManagement.repos[insertIndex]->owner->name, owner->name) == 0 
            && strcmp(repoManagement.repos[insertIndex]->name, repoName) < 0) 
    {
      /*
      if we find a repository whose owner's name is the same as the specified owner's name, 
      traverse the dynamic array of pointers to repositories until we find a repository 
      with the same owner whose name is lexicographically greater than the specified repository name, 
      or we reach the end of the dynamic array
      */
      insertIndex++;
    }
    
    /*
    After finding the correct index,
    shift all repositories after that index to the right by one index to make space for the new repository, and then
    insert the new repository at the index.
    */
    Repository **newReposArray = new Repository*[repoManagement.numRepos + 1]; //create a new dynamic array of pointers to repositories (with size one greater than the current size)
    for (int i = 0; i < insertIndex; i++) //copy all repositories before the insert index from the old dynamic array to the new dynamic array
    {
      newReposArray[i] = repoManagement.repos[i];
    }
    newReposArray[insertIndex] = newRepo; //insert the new repository at the insert index
    for (int i = insertIndex; i < repoManagement.numRepos; i++) //copy all repositories from the insert index to the end of the old dynamic array to the new dynamic array
    {
      newReposArray[i + 1] = repoManagement.repos[i];
    }
    delete[] repoManagement.repos; //deallocate memory allocated to the old dynamic array of pointers to repositories
    repoManagement.repos = newReposArray; //set the dynamic array of pointers to repositories in the repository management structure to the new dynamic array of pointers to repositories
    repoManagement.numRepos++; //increment the number of repositories in the repository management structure by 1
  }

  //add a pointer to the repository in the dynamic array of repositories in the owner user
  if (owner->numRepos == 0) //if the owner has no repositories
  {
    //create a new dynamic array of pointers to repositories
    Repository **newReposArray = new Repository*[1];
    newReposArray[0] = newRepo; //add the new repository to the first index of the new dynamic array of pointers to repositories
    owner->repos = newReposArray; //set the dynamic array of pointers to repositories in the owner user to the new dynamic array of pointers to repositories
    owner->numRepos = 1; //update the number of repositories under the owner user to 1
  }
  else 
  /*
  add a pointer to the existing repository in the dynamic array of repositories under the owner user,
  sorted in lexicographical order of names of the repositories.
  */
  {
    int insertIndex = 0; //initialize the index to insert the new repository at to 0
    while (insertIndex < owner->numRepos && strcmp(owner->repos[insertIndex]->name, repoName) < 0) 
    {
      /*
      traverse the dynamic array of pointers to repositories under the owner user until we find a repository 
      with the same owner whose name is lexicographically greater than the specified repository name, 
      or we reach the end of the dynamic array
      */
      insertIndex++;
    }
    Repository **newReposArray = new Repository*[owner->numRepos + 1]; //create a new dynamic array of pointers to repositories with size one greater than the current size
    for (int i = 0; i < insertIndex; i++) //copy all repositories before the insert index from the old dynamic array to the new dynamic array
    {
      newReposArray[i] = owner->repos[i];
    }
    newReposArray[insertIndex] = newRepo; //insert the new repository at the insert index
    for (int i = insertIndex; i < owner->numRepos; i++) //copy all repositories from the insert index to the end of the old dynamic array to the new dynamic array
    {
      newReposArray[i + 1] = owner->repos[i];
    }
    delete[] owner->repos; //deallocate memory allocated to the old dynamic array of pointers to repositories under the owner user
    owner->repos = newReposArray; //set the dynamic array of pointers to repositories under the owner user to the new dynamic array of pointers to repositories
    owner->numRepos++; //increment the number of repositories under the owner user by 1
  }

  //Returns the index of the newly created repository in the dynamic array of repositories in the repository management structure
  for (int i = 0; i < repoManagement.numRepos; i++) //traverse the dynamic array of pointers to repositories in the repository management structure
  {
    if (repoManagement.repos[i] == newRepo) //check if the current repository is the new repository
    {
      return i; //return the index of the new repository in the dynamic array of repositories in the repository management structure
    }
  }

  // We should never get here if the repository was inserted correctly.
  return -1;
}

/**
 * Task 3 - Create Branch
 *
 * The `create_branch` function creates a branch in the specified repository
 * with the supplied name and creator of the branch, at the specified commit,
 * if no existing branch has the same name already.
 *
 * @param repoManagement: reference to the `RepositoryManagement` structure
 *                        containing the dynamic array of pointers to
 *                        repositories.
 * @param repoFQN: the fully-qualified name of the repository to create a branch
 *                 for.
 * @param branchName: the name of the branch to create.
 * @param creator: pointer to the creator user of the branch
 * @param commit: the commit to create the branch from.
 * @returns: false if any validation failed; true if the branch was created
 *           successfully.
 */
bool create_branch(RepositoryManagement &repoManagement, char *repoFQN,
                   const char *branchName, const User *creator,
                   const Commit *commit) {
  // TODO: Task 3
  
  //Performs validation on the uniqueness and length of the branch names

  if (strlen(branchName) >= MAX_BRANCH_NAME_LEN) { //branch name too long
    cout << "Branch name length exceeded " << (MAX_BRANCH_NAME_LEN - 1) << " characters." << endl;
    return false;
  }

  //find the repository in the system with the specified fully-qualified name
  // strtok writes null terminators into the buffer, so keep the original FQN
  // for error messages and parse a local copy instead.
  char repoFQNCopy[MAX_USER_NAME_LEN + MAX_REPO_NAME_LEN + 2];
  strcpy(repoFQNCopy, repoFQN);
  char* ownerName = strtok(repoFQNCopy, "/"); //extract the owner name from the FQN of the repo
  char* repoName = strtok(nullptr, "/"); //extract the repository name from the FQN of the repo


  //if specified repo doesn't exist
  //case 1: repoManagement has no repositories
  if (repoManagement.numRepos == 0) {
    cout << "Repository " << repoFQN << " does not exist." << endl;
    return false;
  }

  //case 2: repoManagement has at least one repository

  //find the index of the owner user with the specified owner name
  int ownerIndex = -1;

  for (int i = 0; i < repoManagement.numRepos; i++) {//traverse the dynamic array of pointers to repositories in the repoManagement
  
    if (strcmp(repoManagement.repos[i]->owner->name, ownerName) == 0 && strcmp(repoManagement.repos[i]->name, repoName) == 0) {
      //check if the current repository has an owner with the same name as the specified owner name 
      //and the same name as the specified repository name
      ownerIndex = i; //set the owner index to the index of that repository and break out of the loop
      break;
    }
  }

  if (ownerIndex == -1) { //if none is found
    cout << "Repository " << repoFQN << " does not exist." << endl;
    return false;
  }
  else { //if one is found, check if the branch name is unique under that repository
    
    Repository* repo = repoManagement.repos[ownerIndex];
    
    for (int i = 0; i < repo->numBranches; i++) { //traverse the dynamic array of pointers to branches under that repository
      if (strcmp(repo->branches[i]->name, branchName) == 0) { //check if the current branch has the same name as the specified branch name
        cout << "Branch " << repoFQN << ":" << branchName << " already exists." << endl;

        return false; 

      }
    }

    //branch name is unique under the repository, we can create the branch
  
    //initialize the new branch
    Branch* newBranch = new Branch;
    strcpy(newBranch->name, branchName); //set the name of the new branch to the specified branch name
    newBranch->creator = creator; //set the creator of the new branch to the specified creator
    newBranch->repo = repo; //set the repository of the new branch to the repository we found earlier with the specified FQN

    /*
    set the head to a copy of the specified commit the branch originates from.
    If the specified commit is nullptr, use the latest commit of the main branch.
    */

    if (commit != nullptr) { //if the specified commit is NOT nullptr, set the head of the new branch to a copy of the specified commit
      Commit* newHead = new Commit;
      newHead->author = commit->author;
      strcpy(newHead->message, commit->message);
      newHead->timestamp = commit->timestamp;
      newHead->hash = commit->hash;

      /*
      The default branch of any repository is typically called main or master. 
      In this programming assignment this is not included 
      as the commits field in the repository structure handles this invariant. 
      */

      newHead->prev = nullptr;
      newHead->next = nullptr;
      newBranch->head = newHead;
    }
    else { //if the specified commit IS nullptr, set the head of the new branch to a copy of the latest commit of the main branch
      // The repository stores the main history directly in repo->commits, so
      // copy that head commit when no explicit starting commit is provided.
      Commit* newHead = new Commit;
      newHead->author = repo->commits->author;
      strcpy(newHead->message, repo->commits->message);
      newHead->timestamp = repo->commits->timestamp;
      newHead->hash = repo->commits->hash;

      /*
      The default branch of any repository is typically called main or master. 
      In this programming assignment this is not included 
      as the commits field in the repository structure handles this invariant. 
      */
      newHead->prev = nullptr;
      newHead->next = nullptr;
      newBranch->head = newHead;
    
    }
    
    //branch is now initialized, we just need to add it to the dynamic array of branches (sorted in lexicographical order by name) under the repository
    int insertIndex = 0; //initialize the index to insert the new branch at to 0

    while (insertIndex < repo->numBranches && strcmp(repo->branches[insertIndex]->name, branchName) < 0) {
      /*
      traverse the dynamic array of pointers to branches under that repository until 
      we find a branch with a name that is lexicographically greater than the specified branch name, 
      or we reach the end of the dynamic array
      */
      insertIndex++;
    }
    Branch** newBranchesArray = new Branch*[repo->numBranches + 1]; //create a new dynamic array of pointers to branches with size one greater than the current size

    for (int i = 0; i < insertIndex; i++) { //copy all branches before the insert index from the old dynamic array to the new dynamic array
      newBranchesArray[i] = repo->branches[i];
    }

    newBranchesArray[insertIndex] = newBranch; //insert the new branch at the insert index
    
    for (int i = insertIndex; i < repo->numBranches; i++) { //copy all branches from the insert index to the end of the old dynamic array to the new dynamic array
      newBranchesArray[i + 1] = repo->branches[i];
    }
    
    delete[] repo->branches; //deallocate memory allocated to the old dynamic array of pointers to branches under the repository
    repo->branches = newBranchesArray; //set the dynamic array of pointers to branches under the repository to the new dynamic array of pointers to branches
    repo->numBranches++; //increment the number of branches under the repository by 1 
    
    return true; //return true to indicate successful branch creation

  }
}

/**
 * Task 4 - Add Commit
 *
 * The `add_commit` function adds a commit in the specified repository
 * with an optionally-specified branch to add the commit to. The hash
 * of the commit is computed from the author and message of the current
 * commit, as well as those of the previous commit (if any).
 *
 * @param repoManagement: reference to the `RepositoryManagement` structure
 *                        containing the dynamic array of pointers to
 *                        repositories.
 * @param author: the author of the commit.
 * @param repoFQN: fully-qualified name of the repository to add the commit
 *                 to.
 * @param commitMessage: the message of the commit.
 * @param branch: optionally, the branch the commit is added to.
 * @param timestamp: the timestamp when the commit was created.
 */
void add_commit(RepositoryManagement &repoManagement, const User *author,
                char *repoFQN, const char *branch, const char *commitMessage,
                time_t timestamp) {
  // TODO: Task 4

  //if the repository to push a commit to does not exist
  // strtok() mutates the input buffer, so parse a local copy and keep the
  // original repository FQN untouched for later messages.
  
  char repoFQNCopy[MAX_USER_NAME_LEN + MAX_REPO_NAME_LEN + 2];
  strcpy(repoFQNCopy, repoFQN);
  char* ownerName = strtok(repoFQNCopy, "/"); //extract the owner name from the FQN of the repo
  char* repoName = strtok(nullptr, "/"); //extract the repository name from the FQN of the repo
  int repoIndex = -1;

  for (int i = 0; i < repoManagement.numRepos; i++) { 
    //traverse the dynamic array of pointers to repositories in the repoManagement
    if (strcmp(repoManagement.repos[i]->owner->name, ownerName) == 0 && strcmp(repoManagement.repos[i]->name, repoName) == 0) {
      //check if the current repository has an owner with the same name as the specified owner name 
      //and the same name as the specified repository name
      repoIndex = i; //set the repository index to the index of that repository and break out of the loop
      break;
    }
  }
  if (repoIndex == -1) {
    cout << "Repository " << repoFQN << " does not exist." << endl;
    return;
  }

  //NOTE: if branch is NOT specified, branch is a nullptr
  //if the branch to push a commit to does not exist
  Repository* repo = repoManagement.repos[repoIndex];
  Branch* targetBranch = nullptr;

  if (branch != nullptr) { //if a branch IS specified, we need to check if it exists under the repository
    for (int i = 0; i < repo->numBranches; i++) { //traverse the dynamic array of pointers to branches under that repository
      if (strcmp(repo->branches[i]->name, branch) == 0) { //check if the current branch has the same name as the specified branch name
        targetBranch = repo->branches[i]; //set targetBranch to point to that branch and break out of the loop
        break;
      }
    }
    if (targetBranch == nullptr) {
      cout << "Branch " << repoFQN << ":" << branch << " does not exist." << endl;
      return;
    }
  }

  //If we reach here, the specified repository (and branch, if specified) exist 
  //and the commit message is of valid length

  //We can create a new commit and add it to the linked list of the branch when specified; 
  //otherwise, add it to the linked list of the main branch (the commmits linked list in the repository itself).

  //One usually views commits as differences with the previous commit in the version history. 
  //For simplicity's sake, this is skipped in this programming assignment.

  //the doubly linked list of commits for any repository or branch is 
  //sorted in reverse chronological order (decreasing order of the timestamp field).
  

  Commit* newCommit = new Commit; //create a pointer to the new commit
  newCommit->author = author; //set the author of the new commit to the specified author
  newCommit->timestamp = timestamp; //set the timestamp of the new commit to the specified timestamp

  //Performs validation on the length of the commit message and copy the message (truncating if needed)
  if (strlen(commitMessage) >= MAX_COMMIT_MSG_LEN) { //commit message too long
    cout << "Warning: commit message is longer than " << (MAX_COMMIT_MSG_LEN - 1) << " characters and will be truncated." << endl;
    //truncate the commit message to length: MAX_COMMIT_MSG_LEN - 1
    strncpy(newCommit->message, commitMessage, MAX_COMMIT_MSG_LEN - 1);
    newCommit->message[MAX_COMMIT_MSG_LEN - 1] = '\0'; //null terminate
  } 
  else {
    strcpy(newCommit->message, commitMessage); //set the message of the new commit to the specified commit message
  }

  newCommit->prev = nullptr; //initialize prev to nullptr
  newCommit->next = nullptr; //initialize next to nullptr

  //compute the new hash of the commit from the author, message and the same metadata from the previous commit (if any) and set hash to it.
  SHA1 newCommit_hasher;
  initialize(newCommit_hasher);
  input(newCommit_hasher, newCommit->author->name, strlen(newCommit->author->name)); //from name of author
  input(newCommit_hasher, newCommit->message, strlen(newCommit->message)); //from commit message
  input(newCommit_hasher, newCommit->timestamp); //from timestamp of the new commit

  // The hash also depends on the commit that this new one is based on.
  // That previous commit is the current head of the branch or repository.
  if (branch != nullptr) { //if a branch IS specified, the previous commit is the head of that branch
    if (targetBranch->head != nullptr) { //if there is a previous commit under that branch, input its metadata next
      input(newCommit_hasher, targetBranch->head->author->name, strlen(targetBranch->head->author->name));
      input(newCommit_hasher, targetBranch->head->message, strlen(targetBranch->head->message));
      input(newCommit_hasher, targetBranch->head->timestamp);
    }
  }
  else { //if a branch is NOT specified, the previous commit is the head of the main branch
    if (repo->commits != nullptr) { //if there is a previous commit under the main branch, input its metadata next
      input(newCommit_hasher, repo->commits->author->name, strlen(repo->commits->author->name));
      input(newCommit_hasher, repo->commits->message, strlen(repo->commits->message));
      input(newCommit_hasher, repo->commits->timestamp);
    }
  }
  digest(newCommit_hasher); //compute the hash
  newCommit->hash = newCommit_hasher; //set the hash of the new commit to the computed hash

  //add the new commit to the linked list of commits for the branch if specified, or the main branch if not specified, 
  //while maintaining the reverse chronological order of the linked list

  if (branch != nullptr) { //if a branch IS specified, add the new commit to the linked list of commits under that branch
    if (targetBranch->head == nullptr) { //if there are no commits under that branch yet, set the head of that branch to the new commit
      targetBranch->head = newCommit;
    }
    else if (newCommit->timestamp >= targetBranch->head->timestamp) { //if the new commit is more recent than or as recent as the current head of that branch, insert the new commit at the head of the linked list
      newCommit->next = targetBranch->head;
      targetBranch->head->prev = newCommit;
      targetBranch->head = newCommit;
    }
    else { //if the new commit is older than the current head of that branch, find the correct position to insert the new commit in the linked list
      Commit* curr = targetBranch->head;
      while (curr->next != nullptr && curr->next->timestamp > newCommit->timestamp) {
        curr = curr->next; //traverse the linked list until we find a commit with a timestamp that is less than or equal to the timestamp of the new commit, or we reach the end of the linked list
      }
      //insert the new commit after curr
      newCommit->next = curr->next;
      if (curr->next != nullptr) {
        curr->next->prev = newCommit;
      }
      curr->next = newCommit;
      newCommit->prev = curr;
    }
  }
  else { //if a branch is NOT specified, add the new commit to the linked list of commits under the main branch (the commits linked list in the repository itself)
    if (repo->commits == nullptr) { //if there are no commits under the main branch yet, set the head of the main branch to the new commit
      repo->commits = newCommit;
    }
    else if (newCommit->timestamp >= repo->commits->timestamp) { //if the new commit is more recent than or as recent as the current head of the main branch, insert the new commit at the head of the linked list
      newCommit->next = repo->commits;
      repo->commits->prev = newCommit;
      repo->commits = newCommit;
    }
    else { //if the new commit is older than the current head of the main branch, find the correct position to insert the new commit in the linked list
      Commit* curr = repo->commits;
      while (curr->next != nullptr && curr->next->timestamp > newCommit->timestamp) {
        curr = curr->next; //traverse the linked list until we find a commit with a timestamp that is less than or equal to the timestamp of the new commit, or we reach the end of the linked list
      }
      //insert the new commit after curr
      newCommit->next = curr->next;
      if (curr->next != nullptr) {
        curr->next->prev = newCommit;
      }
      curr->next = newCommit;
      newCommit->prev = curr;
    }
  }

  //if branch is specified, print "Pushing commit <hash> to branch <repoFQN>:<branch>."
  //otherwise, print "Pushing commit <hash> to branch <repoFQN>:main."
  if (branch != nullptr) {
    cout << "Pushing commit ";
    print_sha(newCommit->hash);
    cout << " to branch " << repoFQN << ":" << branch << "." << endl;
  } 
  else {
    cout << "Pushing commit ";
    print_sha(newCommit->hash);
    cout << " to branch " << repoFQN << ":main." << endl;
  }

}

/**
 * Task 5 - Transfer Ownership
 *
 * The `transfer_ownership` function transfers the ownership of a repository
 * from its current owner to another user. Both users have to be registered
 * users on the platform.
 *
 * @param userManagement: reference to the `UserManagement` structure containing
 *                        the linked list of users.
 * @param repoManagement: reference to the `RepositoryManagement` structure
 *                        containing the dynamic array of pointers to
 *                        repositories.
 * @param fromUsername: the name of the current owner of the specified
 *                      repsitory.
 * @param toUsername: the new owner of the specified repository.
 * @param repoName: the name of the repository.
 * @returns: true if the ownership transfer was successful; false otherwise.
 */
bool transfer_ownership(UserManagement &userManagement,
        RepositoryManagement &repoManagement,
        const char *fromUsername, const char *toUsername,
        const char *repoName) {
  // TODO: Task 5

  //Performs validation that the target user to grant ownership to exist, 
  //and do nothing if the old and new owners is the same user.

  //if either the user to transfer ownership from, or the target user, does not exist
  User* fromUser = nullptr;
  User* toUser = nullptr;
  User* curr = userManagement.head;
  while (curr != nullptr) { //traverse the linked list of users in the user management structure
    if (strcmp(curr->name, fromUsername) == 0) { //check if the current user has the same name as the specified from username
      fromUser = curr; //set fromUser to point to that user
    }
    if (strcmp(curr->name, toUsername) == 0) { //check if the current user has the same name as the specified to username
      toUser = curr; //set toUser to point to that user
    }
    curr = curr->next;
  }
  if (fromUser == nullptr) {
    cout << "User " << fromUsername << " does not exist." << endl;
    return false;
  }
  if (toUser == nullptr) {
    cout << "User " << toUsername << " does not exist." << endl;
    return false;
  }
  if (fromUser == toUser) {
    cout << "Why are you transferring ownership to the same user?" << endl;
    return false;
  }

  //if the repository to transfer ownership from the original user does not exist.
  Repository* repoToTransfer = nullptr;
  for (int i = 0; i < fromUser->numRepos; i++) { //traverse the dynamic array of pointers to repositories under the original owner user
    if (strcmp(fromUser->repos[i]->name, repoName) == 0) { //check if the current repository has the same name as the specified repository name
      repoToTransfer = fromUser->repos[i]; //set repoToTransfer to point to that repository and break out of the loop
      break;
    }
  }
  if (repoToTransfer == nullptr) {
    cout << "Repository " << fromUsername << "/" << repoName << " not found." << endl;
    return false;
  }

  //if there is already a repository with the same name as the one to transfer ownership, currently owned by the target user.
  for (int i = 0; i < toUser->numRepos; i++) {
    if (strcmp(toUser->repos[i]->name, repoName) == 0) {
      cout << "Repository " << toUsername << "/" << repoName << " already exists." << endl;
      return false;
    }
  }

  //Updates the owner of the specified repository to the specified user
  repoToTransfer->owner = toUser;

  //Removes the repository in question from the repositories owned by the old user.
  
  Repository** newReposArrayFromUser = nullptr;
  if (fromUser->numRepos > 1) { //if the original owner user has more than one repository
    newReposArrayFromUser = new Repository*[fromUser->numRepos - 1]; //create a new dynamic array of pointers to repositories with size one less than the current size for the original owner user
    int indexFromUser = 0; //initialize the index to insert repositories at in the new dynamic array of pointers to repositories under the original owner user to 0
    for (int i = 0; i < fromUser->numRepos; i++) { //traverse the dynamic array of pointers to repositories under the original owner user
      if (fromUser->repos[i] != repoToTransfer) { //if the current repository is not the repository to transfer ownership of, add it to the new dynamic array of pointers to repositories under the original owner user
        newReposArrayFromUser[indexFromUser] = fromUser->repos[i];
        indexFromUser++;
      }
    }
  }
  delete[] fromUser->repos; //deallocate memory allocated to the old dynamic array of pointers to repositories under the original owner user
  fromUser->repos = newReposArrayFromUser; //set the dynamic array of pointers to repositories under the original owner user to the new dynamic array of pointers to repositories
  fromUser->numRepos--; //decrement the number of repositories under the original owner user by 1


  //Adds the repository in question to the repositories owned by the new user. (inserted in the correct position to maintain lexicographical order by repository name)
  
  Repository** newReposArrayToUser = new Repository*[toUser->numRepos + 1]; //create a new dynamic array of pointers to repositories with size one greater than the current size for the target owner user
  int indexToUser = 0; //initialize the index to insert repositories at in the new dynamic array of pointers to repositories under the target owner user to 0
  for (int i = 0; i < toUser->numRepos; i++) { //traverse the dynamic array of pointers to repositories under the target owner user
    if (strcmp(toUser->repos[i]->name, repoToTransfer->name) < 0) { //if the current repository has a name that is lexicographically less than the name of the repository to transfer ownership of, add it to the new dynamic array of pointers to repositories under the target owner user
      newReposArrayToUser[indexToUser] = toUser->repos[i];
      indexToUser++;
    }
    else { //if the current repository has a name that is lexicographically greater than or equal to the name of the repository to transfer ownership of, add the repository to transfer ownership of to the new dynamic array of pointers to repositories under the target owner user before adding the current repository, and then break out of the loop
      newReposArrayToUser[indexToUser] = repoToTransfer;
      indexToUser++;
      for (int j = i; j < toUser->numRepos; j++) { //add the remaining repositories under the target owner user to the new dynamic array of pointers to repositories under the target owner user
        newReposArrayToUser[indexToUser] = toUser->repos[j];
        indexToUser++;
      }
      break;
    }
  }
  if (indexToUser == toUser->numRepos) { //if we have traversed all repositories under the target owner user and the repository to transfer ownership of has a name that is lexicographically greater than all of them, add the repository to transfer ownership of to the end of the new dynamic array of pointers to repositories under the target owner user
    newReposArrayToUser[indexToUser] = repoToTransfer;
  }
  delete[] toUser->repos; //deallocate memory allocated to the old dynamic array of pointers to repositories under the target owner user
  toUser->repos = newReposArrayToUser; //set the dynamic array of pointers to repositories under the target owner user to the new dynamic array of pointers to repositories
  toUser->numRepos++; //increment the number of repositories under the target owner user by 1

  /*
  Maintains the invariant of the dynamic array of repositories. 
  (the dynamic array of pointers to repositories in the repository management structure is 
  sorted first by the name of the owner, and then the name of the repository, both in lexicographical order.)
  */
  Repository** newReposArrayRepoManagement = new Repository*[repoManagement.numRepos]; //create a new dynamic array of pointers to repositories with the same size as the current size for the repository management structure
  int indexRepoManagement = 0; //initialize the index to insert repositories at in the new dynamic array of pointers to repositories in the repository management structure to 0
  for (int i = 0; i < repoManagement.numRepos; i++) { //traverse the dynamic array of pointers to repositories in the repository management structure
    if (repoManagement.repos[i] != repoToTransfer) { 
      //if the current repository is not the repository to transfer ownership of, 
      //add it to the new dynamic array of pointers to repositories in the repo management structure in the correct position to maintain the invariant
      
      if (strcmp(repoManagement.repos[i]->owner->name, repoToTransfer->owner->name) < 0 ||
          (strcmp(repoManagement.repos[i]->owner->name, repoToTransfer->owner->name) == 0 &&
            strcmp(repoManagement.repos[i]->name, repoToTransfer->name) < 0)) {
        
        //if the current repository has an owner name that is lexicographically less than the owner name of the repository to transfer ownership of,
        //or if they have the same owner name and the current repository has a name that is lexicographically less than the name of the repository to transfer ownership of, 
        //add the current repository to the new dynamic array of pointers to repositories in the repository management structure

        newReposArrayRepoManagement[indexRepoManagement] = repoManagement.repos[i];
        indexRepoManagement++;

      } 
      else {

        //if the current repository has an owner name that is lexicographically greater than the owner name of the repository to transfer ownership of,
        //or if they have the same owner name and the current repository has a name that is lexicographically greater than or equal to the name of the repository to transfer ownership of,
        //add the repository to transfer ownership of to the new dynamic array of pointers to repositories in the repository management structure before adding the current repository, and then add the remaining repositories in the repository management
        
        newReposArrayRepoManagement[indexRepoManagement] = repoToTransfer;
        indexRepoManagement++;
        newReposArrayRepoManagement[indexRepoManagement] = repoManagement.repos[i];
        indexRepoManagement++;
        for (int j = i + 1; j < repoManagement.numRepos; j++) {
          if (repoManagement.repos[j] != repoToTransfer) {

            newReposArrayRepoManagement[indexRepoManagement] = repoManagement.repos[j];
            indexRepoManagement++;
          }
        }
        break;
      }
    }
  }
    if (indexRepoManagement == repoManagement.numRepos - 1) {
      newReposArrayRepoManagement[indexRepoManagement] = repoToTransfer;
    }
  delete[] repoManagement.repos; //deallocate memory allocated to the old dynamic array of pointers to repositories in the repository management structure
  repoManagement.repos = newReposArrayRepoManagement; //set the dynamic array of pointers to repositories in the repository management structure to the new dynamic array of pointers to repositories with the updated owner for the repository to transfer ownership of and the same repositories for all other repositories in the repository management structure 
  
  //If we reach here, the ownership transfer is successful
  return true;
}

/**
 * Task 6 - Create Pull Request
 *
 * The `create_pull_request` function creates a pull request from one branch of
 * a repository to another. The repositories can be different.
 *
 * @param repoManagement: reference to the `RepositoryManagement` structure
 *                        containing the dynamic array of pointers to
 *                        repositories.
 * @param title: the creator of the pull request
 * @param author: the author of the pull request
 * @param fromBranchFQN: fully-qualified name of the branch to make a pul
 *                        request from.
 * @param toBranchFQN: fully-qualified name of the branch to merge the
 *                      suggested changes to.
 * @returns: true if the pull request was created successfully.
 */
bool create_pull_request(const RepositoryManagement &repoManagement,
                         const char *title, const User *author,
                         char *fromBranchFQN, char *toBranchFQN) {
  // TODO: Task 6

  //Performs validation on the length of the pull request title.     

  //if the name of the pull request is too long
  if (strlen(title) >= MAX_PR_TITLE_LEN) {
    cout << "Pull request title length exceeded " << (MAX_PR_TITLE_LEN - 1) << " characters." << endl;
    return false;
  }

  //if any related repository in the pull request does not exist, print "Repository <fqn> does not exist."

  // Parse branch FQNs of the form "owner/repo:branch" without mutating
  // the caller buffers. We'll copy the inputs then split on ':' then '/'.
  char fromCopy[MAX_USER_NAME_LEN + MAX_REPO_NAME_LEN + MAX_BRANCH_NAME_LEN + 3];
  char toCopy[MAX_USER_NAME_LEN + MAX_REPO_NAME_LEN + MAX_BRANCH_NAME_LEN + 3];
  strncpy(fromCopy, fromBranchFQN, sizeof(fromCopy) - 1);
  fromCopy[sizeof(fromCopy) - 1] = '\0';
  strncpy(toCopy, toBranchFQN, sizeof(toCopy) - 1);
  toCopy[sizeof(toCopy) - 1] = '\0';

  char* fromRepoPart = strtok(fromCopy, ":"); // "owner/repo"
  char* fromBranchName = strtok(nullptr, ":"); // "branch"
  char* fromOwnerName = nullptr;
  char* fromRepoName = nullptr;
  if (fromRepoPart) {
    fromOwnerName = strtok(fromRepoPart, "/");
    fromRepoName = strtok(nullptr, "/");
  }

  char* toRepoPart = strtok(toCopy, ":"); // "owner/repo"
  char* toBranchName = strtok(nullptr, ":"); // "branch"
  char* toOwnerName = nullptr;
  char* toRepoName = nullptr;
  if (toRepoPart) {
    toOwnerName = strtok(toRepoPart, "/");
    toRepoName = strtok(nullptr, "/");
  }

  Repository* fromRepo = nullptr;
  Repository* toRepo = nullptr;

  for (int i = 0; i < repoManagement.numRepos; i++) { //traverse the dynamic array of pointers to repositories in the repository management structure
    if (strcmp(repoManagement.repos[i]->owner->name, fromOwnerName) == 0 && strcmp(repoManagement.repos[i]->name, fromRepoName) == 0) {
      //check if the current repository has an owner with the same name as the specified owner name for the from branch
      //and has the same name as the specified repository name for the from branch
      fromRepo = repoManagement.repos[i]; //set fromRepo to point to that repository
    }
    if (strcmp(repoManagement.repos[i]->owner->name, toOwnerName) == 0 && strcmp(repoManagement.repos[i]->name, toRepoName) == 0) {
      //check if the current repository has an owner with the same name as the specified owner name for the to branch
      //and has the same name as the specified repository name for the to branch
      toRepo = repoManagement.repos[i]; //set toRepo to point to that repository
    }
  }
  if (fromRepo == nullptr) {
    cout << "Repository " << fromOwnerName << "/" << fromRepoName << " does not exist." << endl;
    return false;
  }
  if (toRepo == nullptr) {
    cout << "Repository " << toOwnerName << "/" << toRepoName << " does not exist." << endl;
    return false;
  }

  //if any branch related in the pull request does not exist, print "Branch <fqn> does not exist."
  Branch* fromBranch = nullptr;
  Branch* toBranch = nullptr;
  for (int i = 0; i < fromRepo->numBranches; i++) { //traverse the dynamic array of pointers to branches under the from repository
    if (strcmp(fromRepo->branches[i]->name, fromBranchName) == 0) { //check if the current branch has the same name as the specified branch name for the from branch
      fromBranch = fromRepo->branches[i]; //set fromBranch to point to that branch
      break;
    }
  }
  for (int i = 0; i < toRepo->numBranches; i++) { //traverse the dynamic array of pointers to branches under the to repository
    if (strcmp(toRepo->branches[i]->name, toBranchName) == 0) { //check if the current branch has the same name as the specified branch name for the to branch
      toBranch = toRepo->branches[i]; //set toBranch to point to that branch
      break;
    }
  }
  if (fromBranch == nullptr) {
    cout << "Branch " << fromOwnerName << "/" << fromRepoName << ":" << fromBranchName << " does not exist." << endl;
    return false;
  }
  if (toBranch == nullptr) {
    // The main branch is implicit (stored in repo->commits) and may not be
    // present in the repo->branches array. Treat "main" as existing.
    if (toBranchName == nullptr || strcmp(toBranchName, "main") != 0) {
      cout << "Branch " << toOwnerName << "/" << toRepoName << ":" << toBranchName << " does not exist." << endl;
      return false;
    }
    // leave toBranch as nullptr to indicate main
  }

  //If we reach here, the pull request can be created successfully
  
  //set *author* to a pointer to the author of the pull request
  //set *id* to the current number of pull requests in the repository + 1
  //set *title* to a copy of the title provided
  //set *repo* to a pointer to the specified repository
  //set *fromBranch* and *toBranch* to the specified branches respectively
  //set *status* to OPEN

  PullRequest* newPR = new PullRequest; //create a pointer to the new pull request
  newPR->author = author; //set the author of the new pull request to the specified author
  newPR->id = toRepo->numPrs + 1; //set the id of the new pull request to the current number of pull requests in the to repository + 1
  strcpy(newPR->title, title); //set the title of the new pull request to a copy of the specified title
  newPR->repo = toRepo; //set the repository of the new pull request to point to the to repository
  newPR->fromBranch = fromBranch; //set fromBranch of the new pull request to point to the from branch
  newPR->toBranch = toBranch; //set toBranch of the new pull request to point to the to branch
  newPR->status = OPEN; //set the status of the new pull request to OPEN

  //add the new pull request to the dynamic array of pointers to pull requests under the repository (sorted in ascending order by ID of the pull request)
  PullRequest** newPRsArray = new PullRequest*[toRepo->numPrs + 1]; //create a new dynamic array of pointers to pull requests with size one greater than the current size for the to repository
  int insertIndex = 0; //initialize the index to insert the new pull request at to 0
  while (insertIndex < toRepo->numPrs && toRepo->prs[insertIndex]->id < newPR->id) {
    /*
    traverse the dynamic array of pointers to pull requests under the to repository until 
    we find a pull request with an ID that is greater than or equal to the ID of the new pull request, 
    or we reach the end of the dynamic array
    */
    insertIndex++;
  }
  for (int i = 0; i < insertIndex; i++) { //copy all pull requests before the insert index from the old dynamic array to the new dynamic array
    newPRsArray[i] = toRepo->prs[i];
  }
  newPRsArray[insertIndex] = newPR; //insert the new pull request at the insert index
  for (int i = insertIndex; i < toRepo->numPrs; i++) { //copy all pull requests from the insert index to the end of the old dynamic array to the new dynamic array
    newPRsArray[i + 1] = toRepo->prs[i];
  }
  delete[] toRepo->prs; //deallocate memory allocated to the old dynamic array of pointers to pull requests under the to repository
  toRepo->prs = newPRsArray; //set the dynamic array of pointers to pull requests under the to repository to the new dynamic array of pointers to pull requests
  toRepo->numPrs++; //increment the number of pull requests under the to repository by 1

  cout << "Pull request #" << newPR->id << " has been created in " << newPR->repo->owner->name << "/" << newPR->repo->name << "." << endl;
  return true; //return true to indicate successful pull request creation
}

/**
 * Task 7 - Fork Repository
 *
 * The `fork_repository` function allows the creation of forks of repositories.
 *
 * @param userManagement: reference to the `UserManagement` structure containing
 *                        the linked list of users.
 * @param repoManagement: reference to the `RepositoryManagement` structure
 *                        containing the dynamic array of pointers to
 *                        repositories.
 * @param owner: the owner of the repository to create the fork from.
 * @param forkedOwner: the ownr of the forked repository.
 * @param repoToFork: the name of the repository to fork.
 * @returns true if the repository was forked successfully; false otherwise.
 */
bool fork_repository(UserManagement &userManagement,
                     RepositoryManagement &repoManagement, const User *owner,
                     const char *forkedOwner, const char *repoToFork) {
  // TODO: Task 7

  // Validate that the source repository exists.
  Repository* repoToForkPtr = nullptr;
  for (int i = 0; i < repoManagement.numRepos; i++) {
    if (strcmp(repoManagement.repos[i]->owner->name, owner->name) == 0 &&
        strcmp(repoManagement.repos[i]->name, repoToFork) == 0) {
      repoToForkPtr = repoManagement.repos[i];
      break;
    }
  }
  if (repoToForkPtr == nullptr) {
    cout << "Repository " << owner->name << "/" << repoToFork << " does not exist." << endl;
    return false;
  }

  // Reject duplicate fork names before creating the new repository.
  for (int i = 0; i < repoManagement.numRepos; i++) {
    if (strcmp(repoManagement.repos[i]->owner->name, forkedOwner) == 0 &&
        strcmp(repoManagement.repos[i]->name, repoToFork) == 0) {
      cout << "Repository " << forkedOwner << "/" << repoToFork << " already exists." << endl;
      return false;
    }
  }

  // Resolve the user who will own the fork.
  User* forkedOwnerPtr = nullptr;
  User* curr = userManagement.head;
  while (curr != nullptr) {
    if (strcmp(curr->name, forkedOwner) == 0) {
      forkedOwnerPtr = curr;
      break;
    }
    curr = curr->next;
  }
  if (forkedOwnerPtr == nullptr) {
    cout << "User " << forkedOwner << " does not exist." << endl;
    return false;
  }

  // Create the fork repository and deep-copy all commits and branches from the base repository.
  Repository* newRepo = new Repository;
  newRepo->owner = forkedOwnerPtr; //set the owner of the fork repository to point to the owner of the fork repository
  strcpy(newRepo->name, repoToForkPtr->name);
  newRepo->numPrs = 0;
  newRepo->prs = nullptr;
  newRepo->numForks = 0;
  newRepo->forks = nullptr;
  newRepo->commits = nullptr;

  // Copy the commits in the base repo
  if (repoToForkPtr->commits != nullptr) {
    Commit* src = repoToForkPtr->commits; 
    Commit* prev = nullptr;
    while (src != nullptr) {
      Commit* c = new Commit; 
      c->author = src->author;
      strcpy(c->message, src->message);
      c->timestamp = src->timestamp;
      c->hash = src->hash;
      c->prev = prev;
      c->next = nullptr;
      if (prev != nullptr) prev->next = c;
      else newRepo->commits = c;
      prev = c;
      src = src->next;
    }
  }

  // Copy all branches and their commit chains.
  newRepo->numBranches = repoToForkPtr->numBranches;
  if (newRepo->numBranches > 0) {
    newRepo->branches = new Branch*[newRepo->numBranches];
  } 
  else {
    newRepo->branches = nullptr;
  }

  for (int i = 0; i < newRepo->numBranches; i++) {
    Branch* new_branch = new Branch;
    strcpy(new_branch->name, repoToForkPtr->branches[i]->name);
    new_branch->creator = repoToForkPtr->branches[i]->creator;
    new_branch->repo = newRepo;
    new_branch->head = nullptr;

    if (repoToForkPtr->branches[i]->head != nullptr) {
      Commit* src = repoToForkPtr->branches[i]->head;
      Commit* prev = nullptr;
      while (src != nullptr) {
        Commit* c = new Commit;
        c->author = src->author;
        strcpy(c->message, src->message);
        c->timestamp = src->timestamp;
        c->hash = src->hash;
        c->prev = prev;
        c->next = nullptr;
        if (prev != nullptr) prev->next = c;
        else new_branch->head = c;
        prev = c;
        src = src->next;
      }
    }
    newRepo->branches[i] = new_branch;
  }

  // Insert the new repository into the global repository list. 
  //(repoManagement.repos is sorted first by owner name, then by repository name, both in lexicographical order)
  Repository** newReposArray = new Repository*[repoManagement.numRepos + 1];
  int insertIndex = 0;
  while (insertIndex < repoManagement.numRepos &&
         strcmp(repoManagement.repos[insertIndex]->owner->name, newRepo->owner->name) < 0) {
    insertIndex++;
  }
  while (insertIndex < repoManagement.numRepos &&
         strcmp(repoManagement.repos[insertIndex]->owner->name, newRepo->owner->name) == 0 &&
         strcmp(repoManagement.repos[insertIndex]->name, newRepo->name) < 0) {
    insertIndex++;
  }

  for (int i = 0; i < insertIndex; i++) {
    newReposArray[i] = repoManagement.repos[i];
  }
  newReposArray[insertIndex] = newRepo;
  for (int i = insertIndex; i < repoManagement.numRepos; i++) {
    newReposArray[i + 1] = repoManagement.repos[i];
  }
  delete[] repoManagement.repos;
  repoManagement.repos = newReposArray;
  repoManagement.numRepos++;

  // Record the fork under the source repository. 
  //(sorted first by the name of the owner, and then the name of the repository, both in lexicographical order)
  Repository** newForksArray = new Repository*[repoToForkPtr->numForks + 1];
  int insertForkIndex = 0;
  while (insertForkIndex < repoToForkPtr->numForks &&
         strcmp(repoToForkPtr->forks[insertForkIndex]->owner->name, newRepo->owner->name) < 0) {
    insertForkIndex++;
  }
  while (insertForkIndex < repoToForkPtr->numForks &&
         strcmp(repoToForkPtr->forks[insertForkIndex]->owner->name, newRepo->owner->name) == 0 &&
         strcmp(repoToForkPtr->forks[insertForkIndex]->name, newRepo->name) < 0) {
    insertForkIndex++;
  }

  for (int i = 0; i < insertForkIndex; i++) {
    newForksArray[i] = repoToForkPtr->forks[i];
  }
  newForksArray[insertForkIndex] = newRepo;
  for (int i = insertForkIndex; i < repoToForkPtr->numForks; i++) {
    newForksArray[i + 1] = repoToForkPtr->forks[i];
  }
  delete[] repoToForkPtr->forks;
  repoToForkPtr->forks = newForksArray;
  repoToForkPtr->numForks++;

  // Add the fork to the new owner's personal repository list.
  //sorted in lexicographical order by repository name
  Repository** newReposArrayOwner = new Repository*[forkedOwnerPtr->numRepos + 1];
  int insertOwnerIndex = 0;
  while (insertOwnerIndex < forkedOwnerPtr->numRepos &&
         strcmp(forkedOwnerPtr->repos[insertOwnerIndex]->name, newRepo->name) < 0) {
    insertOwnerIndex++;
  }

  for (int i = 0; i < insertOwnerIndex; i++) {
    newReposArrayOwner[i] = forkedOwnerPtr->repos[i];
  }
  newReposArrayOwner[insertOwnerIndex] = newRepo;
  for (int i = insertOwnerIndex; i < forkedOwnerPtr->numRepos; i++) {
    newReposArrayOwner[i + 1] = forkedOwnerPtr->repos[i];
  }
  delete[] forkedOwnerPtr->repos;
  forkedOwnerPtr->repos = newReposArrayOwner;
  forkedOwnerPtr->numRepos++;

  cout << "Fork " << forkedOwnerPtr->name << "/" << newRepo->name << " created successfully." << endl;
  return true;
}

/**
 * Task 8.1 - Merge Pull Request (Squash Merge)
 *
 * The `merge_pull_request_squashmerge` function merges the specified
 * pull request in a repsitory using the squash merge strategy
 * (combines all commits in the pull request into one and add it to the target
 * branch).
 *
 * @param repoManagement: reference to the `RepositoryManagement` structure
 *                        containing the dynamic array of pointers to
 *                        repositories.
 * @param repoFQN: the name of the repository to merge a pull request for.
 * @param prNumber: the number of the pull request to merge.
 * @param timestamp: the timestamp when this pull request was merged.
 */
void merge_pull_request_squashmerge(RepositoryManagement &repoManagement,
                                    char *repoFQN, int prNumber,
                                    time_t timestamp) {
  // TODO: Task 8.1

  //extract the owner name and repository name from the provided repository FQN
  char *ownerName = strtok(repoFQN, "/");
  char *repoName = strtok(nullptr, "/");

  //Performs validation that the provided pull request number is not out of range for the specified repository
  //if any related repository in the pull request does not exist'
  Repository* repo = nullptr;
  for (int i = 0; i < repoManagement.numRepos; i++) {
    if (strcmp(repoManagement.repos[i]->owner->name, ownerName) == 0 &&
        strcmp(repoManagement.repos[i]->name, repoName) == 0) {
      repo = repoManagement.repos[i];
      break;
    }
  }

  if (!repo) {
    cout << "Repository " << ownerName << "/" << repoName << " does not exist." << endl;
    return;
  }

  //if the specified pull request number is out of range.
  if (prNumber < 1 || prNumber > repo->numPrs) {
    cout << "Invalid pull request number for repository " << ownerName << "/" << repoName << ": out of range." << endl;
    return;
  }
  
  //If we reach here, the pull request can be merged successfully using the squash merge strategy
  //Merge the specified pull request by adding a commit to the target branch of the pull request, with title <title> (#<number>), where:
  //<number> is the unique ID of the pull request; 
  //<title> is the title of the pull request.

  PullRequest *prToMerge = repo->prs[prNumber - 1];

  // Build the squash-merge commit message 
  char commitMessage[MAX_COMMIT_MSG_LEN];
  strcpy(commitMessage, prToMerge->title);
  strcat(commitMessage, " (#");

  // Convert the pull request id to characters in a small temporary buffer.
  char idBuffer[16];
  int idValue = prToMerge->id;
  int idLength = 0;
  do {
    idBuffer[idLength++] = '0' + (idValue % 10);
    idValue /= 10;
  } while (idValue > 0);

  // Reverse the digits so they appear in the normal order.
  for (int left = 0, right = idLength - 1; left < right; left++, right--) {
    char temp = idBuffer[left];
    idBuffer[left] = idBuffer[right];
    idBuffer[right] = temp;
  }
  idBuffer[idLength] = '\0';

  strcat(commitMessage, idBuffer);
  strcat(commitMessage, ")");

  // Build the repository FQN for the target branch repository.
  char targetRepoFQN[MAX_USER_NAME_LEN + MAX_REPO_NAME_LEN + 2];
  strcpy(targetRepoFQN, prToMerge->repo->owner->name);
  strcat(targetRepoFQN, "/");
  strcat(targetRepoFQN, prToMerge->repo->name);

  // Add the squash-merge commit to the target branch using the pull request
  // author, the computed message, and the provided merge timestamp.

  // Create and insert the commit manually (do not call add_commit) so that
  // the merge operation does not print a separate "Pushing commit ..." line.
  Commit* newCommit = new Commit;
  newCommit->author = prToMerge->author;
  strcpy(newCommit->message, commitMessage);
  newCommit->timestamp = timestamp;
  newCommit->prev = nullptr;
  newCommit->next = nullptr;

  // compute hash similar to add_commit
  SHA1 hasher;
  initialize(hasher);
  input(hasher, newCommit->author->name, strlen(newCommit->author->name));
  input(hasher, newCommit->message, strlen(newCommit->message));
  input(hasher, newCommit->timestamp);

  Branch* targetBranch = (prToMerge->toBranch ? prToMerge->toBranch : nullptr);
  // include previous commit metadata if present
  if (targetBranch) {
    if (targetBranch->head != nullptr) {
      input(hasher, targetBranch->head->author->name, strlen(targetBranch->head->author->name));
      input(hasher, targetBranch->head->message, strlen(targetBranch->head->message));
      input(hasher, targetBranch->head->timestamp);
    }
  } 
  else {
    if (repo->commits != nullptr) {
      input(hasher, repo->commits->author->name, strlen(repo->commits->author->name));
      input(hasher, repo->commits->message, strlen(repo->commits->message));
      input(hasher, repo->commits->timestamp);
    }
  }
  digest(hasher);
  newCommit->hash = hasher;

  // insert into the correct commit list while maintaining reverse-chronological order
  if (targetBranch) {
    if (targetBranch->head == nullptr) {
      targetBranch->head = newCommit;
    } 
    else if (newCommit->timestamp >= targetBranch->head->timestamp) {
      newCommit->next = targetBranch->head;
      targetBranch->head->prev = newCommit;
      targetBranch->head = newCommit;
    } 
    else {
      Commit* curr = targetBranch->head;
      while (curr->next != nullptr && curr->next->timestamp > newCommit->timestamp) {
        curr = curr->next;
      }
      newCommit->next = curr->next;

      if (curr->next != nullptr) curr->next->prev = newCommit;
      curr->next = newCommit;
      newCommit->prev = curr;
    }
  } 
  else {
    if (repo->commits == nullptr) {
      repo->commits = newCommit;
    } 
    else if (newCommit->timestamp >= repo->commits->timestamp) {
      newCommit->next = repo->commits;
      repo->commits->prev = newCommit;
      repo->commits = newCommit;
    } 
    else {
      Commit* curr = repo->commits;
      while (curr->next != nullptr && curr->next->timestamp > newCommit->timestamp) {
        curr = curr->next;
      }
      newCommit->next = curr->next;
      if (curr->next != nullptr) curr->next->prev = newCommit;
      curr->next = newCommit;
      newCommit->prev = curr;
    }
  }

  // Mark the pull request as merged before removing the source branch.
  prToMerge->status = MERGED;

  // Delete the branch the pull request came from, along with its commit chain.
  Branch *sourceBranch = prToMerge->fromBranch;
  Repository *sourceRepo = sourceBranch->repo;

  // First free the commits that belong only to the source branch.
  Commit *currCommit = sourceBranch->head;
  while (currCommit != nullptr) {
    Commit *nextCommit = currCommit->next;
    delete currCommit;
    currCommit = nextCommit;
  }

  // Remove the source branch from its repository's branch array.
  int branchIndex = -1;
  for (int i = 0; i < sourceRepo->numBranches; i++) {
    if (sourceRepo->branches[i] == sourceBranch) {
      branchIndex = i;
      break;
    }
  }

  if (branchIndex != -1) {
    Branch **newBranchesArray = nullptr;
    if (sourceRepo->numBranches > 1) {
      newBranchesArray = new Branch*[sourceRepo->numBranches - 1];
      int newIndex = 0;
      for (int i = 0; i < sourceRepo->numBranches; i++) {
        if (i == branchIndex) {
          continue;
        }
        newBranchesArray[newIndex++] = sourceRepo->branches[i];
      }
    }

    delete[] sourceRepo->branches;
    sourceRepo->branches = newBranchesArray;
    sourceRepo->numBranches--;
  }

  delete sourceBranch;

  cout << "Pull request #" << prToMerge->id << " in "
       << repo->owner->name << "/" << repo->name
       << " has been merged using squash merge." << endl;

}

/**
 * Task 8.2 - Merge Pull Request (Rebase Merge)
 *
 * The `merge_pull_request_rebasemerge` function merges the specified
 * pull request in a repsitory using the rebase merge strategy
 * (rebases all commits in the pull request to the target branch).
 *
 * @param repoManagement: reference to the `RepositoryManagement` structure
 *                        containing the dynamic array of pointers to
 *                        repositories.
 * @param repoFQN: the name of the repository to merge a pull request for.
 * @param prNumber: the number of the pull request to merge.
 * @param timestamp: the timestamp when this pull request was merged.
 */
void merge_pull_request_rebasemerge(RepositoryManagement &repoManagement,
                                    char *repoFQN, int prNumber,
                                    time_t timestamp) {
  // TODO: Task 8.2

  // Split the repo FQN into owner and repo name.
  char *ownerName = strtok(repoFQN, "/");
  char *repoName = strtok(nullptr, "/");

  // Find the repo that contains the pull request.
  Repository *repo = nullptr;
  for (int i = 0; i < repoManagement.numRepos; i++) {
    if (strcmp(repoManagement.repos[i]->owner->name, ownerName) == 0 &&
        strcmp(repoManagement.repos[i]->name, repoName) == 0) {
      repo = repoManagement.repos[i];
      break;
    }
  }

  // If the repository does not exist, stop here.
  if (!repo) {
    cout << "Repository " << ownerName << "/" << repoName << " does not exist." << endl;
    return;
  }

  // If the pull request number is invalid, stop here.
  if (prNumber < 1 || prNumber > repo->numPrs) {
    cout << "Invalid pull request number for repository " << ownerName << "/" << repoName << ": out of range." << endl;
    return;
  }

  //Merge the specified pull request by replaying all commits from the source branch that are not on the target branch. 
  //Use the specified timestamp for all the commits.
  //Recompute the hashes of the added commits.

  // The pull request is stored in the repository's pull request list.
  PullRequest *prToMerge = repo->prs[prNumber - 1];
  Branch *sourceBranch = prToMerge->fromBranch;
  Branch *targetBranch = prToMerge->toBranch;

  // Build the fully-qualified name of the target repository.
  char targetRepoFQN[MAX_USER_NAME_LEN + MAX_REPO_NAME_LEN + 2];
  strcpy(targetRepoFQN, prToMerge->repo->owner->name);
  strcat(targetRepoFQN, "/");
  strcat(targetRepoFQN, prToMerge->repo->name);

  // Count how many commits from the source branch are NOT already on the target branch.
  // We compare commits by hash, because two commits with the same contents should be
  // treated as the same logical commit.
  int commitCountToReplay = 0;
    for (Commit *sourceCommit = sourceBranch->head; sourceCommit != nullptr;
      sourceCommit = sourceCommit->next) {
      bool foundOnTarget = false;
      
      // iterate over commits on target (branch.head or repo.commits for main)
      Commit *targetHead = (targetBranch ? targetBranch->head : prToMerge->repo->commits);
      for (Commit *targetCommit = targetHead; targetCommit != nullptr;
        targetCommit = targetCommit->next) {
        if (hashes_equal(sourceCommit->hash, targetCommit->hash)) {
          foundOnTarget = true;
          break;
        }
      }

      if (!foundOnTarget) {
        commitCountToReplay++;
      }
    }

  // Store the source-only commits so we can replay them in the correct order.
  Commit **commitsToReplay = nullptr;
  if (commitCountToReplay > 0) {
    commitsToReplay = new Commit*[commitCountToReplay];
    int replayIndex = 0;

    for (Commit *sourceCommit = sourceBranch->head; sourceCommit != nullptr;
         sourceCommit = sourceCommit->next) {
      
      bool foundOnTarget = false;
      Commit *targetHead = (targetBranch ? targetBranch->head : prToMerge->repo->commits);
      
      for (Commit *targetCommit = targetHead; targetCommit != nullptr;
        targetCommit = targetCommit->next) {
        if (hashes_equal(sourceCommit->hash, targetCommit->hash)) {
          foundOnTarget = true;
          break;
        }
      }

      if (!foundOnTarget) {
        commitsToReplay[replayIndex] = sourceCommit;
        replayIndex++;
      }
    }

    // Rebase applies commits from oldest to newest.
    // Since our lists are stored newest-first, we replay them backwards.
    for (int i = commitCountToReplay - 1; i >= 0; i--) {
      char replayRepoFQN[MAX_USER_NAME_LEN + MAX_REPO_NAME_LEN + 2];
      strcpy(replayRepoFQN, targetRepoFQN);

      // Insert replayed commit manually to avoid printing
      Commit* src = commitsToReplay[i];
      Commit* newCommit = new Commit;
      newCommit->author = src->author;
      strcpy(newCommit->message, src->message);
      // rebase-replayed commits use the merge timestamp (when they are replayed)
      newCommit->timestamp = timestamp;
      newCommit->prev = nullptr;
      newCommit->next = nullptr;

      // Recompute the hash using the commit's new position on the target branch.
      SHA1 hasher;
      initialize(hasher);
      input(hasher, newCommit->author->name, strlen(newCommit->author->name));
      input(hasher, newCommit->message, strlen(newCommit->message));
      input(hasher, newCommit->timestamp);

      Commit *currentTargetHead = (targetBranch ? targetBranch->head : repo->commits);
      if (currentTargetHead != nullptr) {
        input(hasher, currentTargetHead->author->name, strlen(currentTargetHead->author->name));
        input(hasher, currentTargetHead->message, strlen(currentTargetHead->message));
        input(hasher, currentTargetHead->timestamp);
      }

      digest(hasher);
      newCommit->hash = hasher;

      // insert into target
      if (targetBranch) {
        if (targetBranch->head == nullptr) {
          targetBranch->head = newCommit;
        } 
        else if (newCommit->timestamp >= targetBranch->head->timestamp) {
          newCommit->next = targetBranch->head;
          targetBranch->head->prev = newCommit;
          targetBranch->head = newCommit;
        } 
        else {
          Commit* curr = targetBranch->head;
          while (curr->next != nullptr && curr->next->timestamp > newCommit->timestamp) curr = curr->next;
          newCommit->next = curr->next;
          if (curr->next != nullptr) curr->next->prev = newCommit;
          curr->next = newCommit;
          newCommit->prev = curr;
        }
      } 
      else {
        if (repo->commits == nullptr) {
          repo->commits = newCommit;
        } 
        else if (newCommit->timestamp >= repo->commits->timestamp) {
          newCommit->next = repo->commits;
          repo->commits->prev = newCommit;
          repo->commits = newCommit;
        } 
        else {
          Commit* curr = repo->commits;
          while (curr->next != nullptr && curr->next->timestamp > newCommit->timestamp) curr = curr->next;
          newCommit->next = curr->next;
          if (curr->next != nullptr) curr->next->prev = newCommit;
          curr->next = newCommit;
          newCommit->prev = curr;
        }
      }
    }

    delete[] commitsToReplay;
  }

  // The pull request itself is now considered merged.
  prToMerge->status = MERGED;

  // Remove the source branch from its repository.
  int branchIndex = -1;
  for (int i = 0; i < sourceBranch->repo->numBranches; i++) {
    if (sourceBranch->repo->branches[i] == sourceBranch) {
      branchIndex = i;
      break;
    }
  }

  if (branchIndex != -1) {
    Branch **newBranchesArray = nullptr;
    if (sourceBranch->repo->numBranches > 1) {
      newBranchesArray = new Branch*[sourceBranch->repo->numBranches - 1];
      int newIndex = 0;
      for (int i = 0; i < sourceBranch->repo->numBranches; i++) {
        if (i == branchIndex) {
          continue;
        }
        newBranchesArray[newIndex] = sourceBranch->repo->branches[i];
        newIndex++;
      }
    }

    delete[] sourceBranch->repo->branches;
    sourceBranch->repo->branches = newBranchesArray;
    sourceBranch->repo->numBranches--;
  }

  // Delete the commits that belonged to the source branch.
  Commit *currCommit = sourceBranch->head;
  while (currCommit != nullptr) {
    Commit *nextCommit = currCommit->next;
    delete currCommit;
    currCommit = nextCommit;
  }
  delete sourceBranch;

  cout << "Pull request #" << prToMerge->id << " in "
       << repo->owner->name << "/" << repo->name
       << " has been merged using rebase merge." << endl;
}

/**
 * Task 8.3 - Merge Pull Request (Merge Commit)
 *
 * The `merge_pull_request_mergecommit` function merges the specified
 * pull request in a repsitory using the merge commit strategy
 * (adds all commits to the target branch preserving chronological order, with a
 * final merge commit added).
 *
 * This is slightly different from what actually happens when a merge commit
 * is used, but for simplicity's sake this is done instead.
 *
 * @param repoManagement: reference to the `RepositoryManagement` structure
 *                        containing the dynamic array of pointers to
 *                        repositories.
 * @param repoFQN: the name of the repository to merge a pull request for.
 * @param prNumber: the number of the pull request to merge.
 * @param timestamp: the timestamp when this pull request was merged.
 */
void merge_pull_request_mergecommit(RepositoryManagement &repoManagement,
                                    char *repoFQN, int prNumber,
                                    time_t timestamp) {
  // TODO: Task 8.3

   
  //Step 1: Parse and Validate Repository
  
  // Extract the owner name and repository name from the fully-qualified name (FQN).
  // The FQN is in the format "owner/repo", so we split by "/" to get both parts.
  char *ownerName = strtok(repoFQN, "/");
  char *repoName = strtok(nullptr, "/");

  // Search through all repositories to find the one that matches the owner and repo name.
  Repository *repo = nullptr;
  for (int i = 0; i < repoManagement.numRepos; i++) {
    if (strcmp(repoManagement.repos[i]->owner->name, ownerName) == 0 &&
        strcmp(repoManagement.repos[i]->name, repoName) == 0) {
      repo = repoManagement.repos[i];
      break;
    }
  }

  // If the repository was not found, print an error message and stop.
  if (!repo) {
    cout << "Repository " << ownerName << "/" << repoName << " does not exist." << endl;
    return;
  }

  // Step 2: Validate Pull Request Number
  
  // Check if the pull request number is within the valid range (1 to numPrs).
  // Pull request numbers are 1-indexed, so they range from 1 to the number of PRs.
  if (prNumber < 1 || prNumber > repo->numPrs) {
    cout << "Invalid pull request number for repository " << ownerName << "/" << repoName << ": out of range." << endl;
    return;
  }


  // Step 3: Get Pull Request and Branch Information
  
  // Get the pull request we want to merge. Since PR numbers are 1-indexed but arrays are 0-indexed,
  // we subtract 1 from the PR number to get the correct array index.
  PullRequest *prToMerge = repo->prs[prNumber - 1];
  
  // Get pointers to the source and target branches of the pull request.
  // Source branch = branch we're merging FROM
  // Target branch = branch we're merging INTO
  Branch *sourceBranch = prToMerge->fromBranch;
  Branch *targetBranch = prToMerge->toBranch;
  // Build the fully-qualified name (owner/repo) of the target repository.
  // Use the repository recorded in the pull request (works for main when toBranch is nullptr).
  char targetRepoFQN[MAX_USER_NAME_LEN + MAX_REPO_NAME_LEN + 2];
  strcpy(targetRepoFQN, prToMerge->repo->owner->name);
  strcat(targetRepoFQN, "/");
  strcat(targetRepoFQN, prToMerge->repo->name);

  // Determine the head of the target for comparisons: branch head or repo.commits for main.
  Commit *targetHead = (targetBranch ? targetBranch->head : prToMerge->repo->commits);

  
  // Step 4: Identify Source-Only Commits
  
  // A source-only commit is a commit that exists in the source branch
  // but NOT in the target branch. We'll merge only these commits.
  //
  // We compare commits by their HASH, not by pointer. This is important because
  // two commits with identical contents (same author, message, timestamp) will have
  // the same hash, and should be treated as the same logical commit.
  
  // Count how many commits from the source branch are NOT already on the target branch.
  int commitCountToMerge = 0;
  for (Commit *sourceCommit = sourceBranch->head; sourceCommit != nullptr;
       sourceCommit = sourceCommit->next) {
    
    // Assume this commit is not on the target branch
    bool foundOnTarget = false;
    
    // Check every commit on the target branch to see if we find a matching hash
    for (Commit *targetCommit = targetHead; targetCommit != nullptr;
          targetCommit = targetCommit->next) {
      
      // If the hashes match, this commit is already on the target branch
      if (hashes_equal(sourceCommit->hash, targetCommit->hash)) {
        foundOnTarget = true;
        break;
      }
    }
    
    // If we didn't find this commit on the target branch, count it
    if (!foundOnTarget) {
      commitCountToMerge++;
    }
  }

  // Step 5: Collect and Add Source-Only Commits
  
  // If there are commits to merge, collect them and add them to the target branch.
  Commit **commitsToMerge = nullptr;
  if (commitCountToMerge > 0) {
    // Allocate an array to store pointers to the source-only commits
    commitsToMerge = new Commit*[commitCountToMerge];
    int mergeIndex = 0;

    // Collect the source-only commits by iterating through the source branch
    for (Commit *sourceCommit = sourceBranch->head; sourceCommit != nullptr;
         sourceCommit = sourceCommit->next) {
      
      // Check if this commit is on the target branch
      bool foundOnTarget = false;

      for (Commit *targetCommit = targetHead; targetCommit != nullptr;
        targetCommit = targetCommit->next) {
        if (hashes_equal(sourceCommit->hash, targetCommit->hash)) {
          foundOnTarget = true;
          break;
        }
      }

      // If this commit is not on the target branch, store it in our array
      if (!foundOnTarget) {
        commitsToMerge[mergeIndex] = sourceCommit;
        mergeIndex++;
      }
    }

    // ***Important: Commit Order for Merge Commit
    // 
    // In a merge commit strategy, we want to add commits in CHRONOLOGICAL ORDER
    // (oldest to newest), so the commit history is preserved properly.
    //
    // Our commit lists are stored NEWEST-FIRST (reverse chronological).
    // So we collected commits newest-first, and we need to add them oldest-first.
    // Therefore, we iterate through the collected commits BACKWARDS.
    
    for (int i = commitCountToMerge - 1; i >= 0; i--) {
      // Add this commit to the target branch using its ORIGINAL timestamp,
      // not the merge timestamp. This preserves the chronological order of commits.
      // Insert the commit manually (do not call add_commit) to avoid printing.
      Commit* src = commitsToMerge[i];
      Commit* newCommit = new Commit;
      newCommit->author = src->author;
      strcpy(newCommit->message, src->message);
      newCommit->timestamp = src->timestamp;
      newCommit->prev = nullptr;
      newCommit->next = nullptr;

      // compute hash
      SHA1 hasher;
      initialize(hasher);
      input(hasher, newCommit->author->name, strlen(newCommit->author->name));
      input(hasher, newCommit->message, strlen(newCommit->message));
      input(hasher, newCommit->timestamp);
      // previous commit metadata depends on current target head
      Commit* currentTargetHead = (targetBranch ? targetBranch->head : repo->commits);
      if (currentTargetHead != nullptr) {
        input(hasher, currentTargetHead->author->name, strlen(currentTargetHead->author->name));
        input(hasher, currentTargetHead->message, strlen(currentTargetHead->message));
        input(hasher, currentTargetHead->timestamp);
      }
      digest(hasher);
      newCommit->hash = hasher;

      // insert into target
      if (targetBranch) {
        if (targetBranch->head == nullptr) {
          targetBranch->head = newCommit;
        } 
        else if (newCommit->timestamp >= targetBranch->head->timestamp) {
          newCommit->next = targetBranch->head;
          targetBranch->head->prev = newCommit;
          targetBranch->head = newCommit;
        } 
        else {
          Commit* curr = targetBranch->head;
          while (curr->next != nullptr && curr->next->timestamp > newCommit->timestamp) curr = curr->next;
          newCommit->next = curr->next;
          if (curr->next != nullptr) curr->next->prev = newCommit;
          curr->next = newCommit;
          newCommit->prev = curr;
        }
      } 
      else {
        if (repo->commits == nullptr) {
          repo->commits = newCommit;
        } 
        else if (newCommit->timestamp >= repo->commits->timestamp) {
          newCommit->next = repo->commits;
          repo->commits->prev = newCommit;
          repo->commits = newCommit;
        } 
        else {
          Commit* curr = repo->commits;
          while (curr->next != nullptr && curr->next->timestamp > newCommit->timestamp) curr = curr->next;
          newCommit->next = curr->next;
          if (curr->next != nullptr) curr->next->prev = newCommit;
          curr->next = newCommit;
          newCommit->prev = curr;
        }
      }
    }

    // Free the memory we allocated for the commits array
    delete[] commitsToMerge;
  }

  // Step 6: Build and Add the Merge Commit
  
  // After all source commits are added, we add a special "merge commit" that records
  // the fact that a merge occurred. The message format is:
  // "Merge pull request #<number> from branch <source_branch>"
  
  char mergeMessage[MAX_COMMIT_MSG_LEN];
  strcpy(mergeMessage, "Merge pull request #");

  // Convert the pull request ID from an integer to a string of characters.
  // We build the string in reverse order initially, then reverse it.
  char idBuffer[16];
  int idValue = prToMerge->id;
  int idLength = 0;
  
  // Extract each digit by repeatedly dividing by 10
  do {
    idBuffer[idLength++] = '0' + (idValue % 10);
    idValue /= 10;
  } while (idValue > 0);

  // Reverse the digits so they appear in the correct order
  for (int left = 0, right = idLength - 1; left < right; left++, right--) {
    char temp = idBuffer[left];
    idBuffer[left] = idBuffer[right];
    idBuffer[right] = temp;
  }
  idBuffer[idLength] = '\0';

  // Complete the merge message: "Merge pull request #<id> from branch <branch>"
  strcat(mergeMessage, idBuffer);
  strcat(mergeMessage, " from branch ");
  // use fully-qualified source branch name: owner/repo:branch
  char sourceBranchFQN[MAX_USER_NAME_LEN + MAX_REPO_NAME_LEN + 3 + MAX_BRANCH_NAME_LEN];
  strcpy(sourceBranchFQN, sourceBranch->repo->owner->name);
  strcat(sourceBranchFQN, "/");
  strcat(sourceBranchFQN, sourceBranch->repo->name);
  strcat(sourceBranchFQN, ":");
  strcat(sourceBranchFQN, sourceBranch->name);
  strcat(mergeMessage, sourceBranchFQN);

  // Add the merge commit to the target branch using the MERGE TIMESTAMP,
  // since this is the time when the merge actually happened.
  // Insert merge commit manually to avoid printing
  {
    Commit* newCommit = new Commit;
    newCommit->author = prToMerge->author;
    strcpy(newCommit->message, mergeMessage);
    newCommit->timestamp = timestamp;
    newCommit->prev = nullptr;
    newCommit->next = nullptr;

    SHA1 hasher;
    initialize(hasher);
    input(hasher, newCommit->author->name, strlen(newCommit->author->name));
    input(hasher, newCommit->message, strlen(newCommit->message));
    input(hasher, newCommit->timestamp);
    
    Commit* currentTargetHead = (targetBranch ? targetBranch->head : repo->commits);
    
    if (currentTargetHead != nullptr) {
      input(hasher, currentTargetHead->author->name, strlen(currentTargetHead->author->name));
      input(hasher, currentTargetHead->message, strlen(currentTargetHead->message));
      input(hasher, currentTargetHead->timestamp);
    }
    digest(hasher);
    newCommit->hash = hasher;

    if (targetBranch) {
      if (targetBranch->head == nullptr) {
        targetBranch->head = newCommit;
      } 
      else if (newCommit->timestamp >= targetBranch->head->timestamp) {
        newCommit->next = targetBranch->head;
        targetBranch->head->prev = newCommit;
        targetBranch->head = newCommit;
      } 
      else {
        Commit* curr = targetBranch->head;
        while (curr->next != nullptr && curr->next->timestamp > newCommit->timestamp) curr = curr->next;
        newCommit->next = curr->next;
        if (curr->next != nullptr) curr->next->prev = newCommit;
        curr->next = newCommit;
        newCommit->prev = curr;
      }
    } 
    else {
      if (repo->commits == nullptr) {
        repo->commits = newCommit;
      } 
      else if (newCommit->timestamp >= repo->commits->timestamp) {
        newCommit->next = repo->commits;
        repo->commits->prev = newCommit;
        repo->commits = newCommit;
      } 
      else {
        Commit* curr = repo->commits;
        while (curr->next != nullptr && curr->next->timestamp > newCommit->timestamp) curr = curr->next;
        newCommit->next = curr->next;
        if (curr->next != nullptr) curr->next->prev = newCommit;
        curr->next = newCommit;
        newCommit->prev = curr;
      }
    }
  }

  // Step 7: Mark Pull Request as Merged

  // Update the pull request status to MERGED to indicate it has been processed.
  prToMerge->status = MERGED;


  // Step 8: Delete Source Branch
  
  // Find the index of the source branch in its repository's branch array
  int branchIndex = -1;
  for (int i = 0; i < sourceBranch->repo->numBranches; i++) {
    if (sourceBranch->repo->branches[i] == sourceBranch) {
      branchIndex = i;
      break;
    }
  }

  // Remove the branch from the repository's branch array
  if (branchIndex != -1) {
    Branch **newBranchesArray = nullptr;
    
    // If there are other branches, create a new array without this branch
    if (sourceBranch->repo->numBranches > 1) {
      newBranchesArray = new Branch*[sourceBranch->repo->numBranches - 1];
      int newIndex = 0;
      
      // Copy all branches except the one we're deleting
      for (int i = 0; i < sourceBranch->repo->numBranches; i++) {
        if (i == branchIndex) {
          continue; // Skip the branch we're deleting
        }
        newBranchesArray[newIndex] = sourceBranch->repo->branches[i];
        newIndex++;
      }
    }

    // Replace the old branch array with the new one
    delete[] sourceBranch->repo->branches;
    sourceBranch->repo->branches = newBranchesArray;
    sourceBranch->repo->numBranches--;
  }

  // Step 9: Free Commit Memory
  
  // Delete all commits that belonged to the source branch.
  // We walk through the doubly-linked list of commits and free each one.
  Commit *currCommit = sourceBranch->head;
  while (currCommit != nullptr) {
    Commit *nextCommit = currCommit->next;
    delete currCommit;
    currCommit = nextCommit;
  }
  
  // Finally, delete the branch structure itself
  delete sourceBranch;


  // Step 10: Print Success Message
  
  cout << "Pull request #" << prToMerge->id << " in "
       << repo->owner->name << "/" << repo->name
       << " has been merged using a merge commit." << endl;
}

/**
 * Task 9 - De-register User
 *
 * The `deregister_user` function de-registers a user from the version control
 * system and updates any references to it with the ghost user. This deletes all
 * their repositories and then deallocates memory allocated to this user.
 *
 * @param userManagement: reference to the `UserManagement` structure containing
 *                        the linked list of users.
 * @param repoManagement: reference to the `RepositoryManagement` structure
 *                        containing the dynamic array of pointers to
 *                        repositopries.
 * @param username: the name of the user to de-register.
 */
void deregister_user(UserManagement &userManagement,
                     RepositoryManagement &repoManagement,
                     const char *username) {
  // TODO: Task 9

  // Step 1: Find the User to Deregister
  
  // Search through the linked list of users to find the user with the specified username.
  User *userToDeregister = nullptr;
  User *prevUser = nullptr; // We also track the previous user so we can remove them from the list later
  
  User *curr = userManagement.head;
  while (curr != nullptr) {
    if (strcmp(curr->name, username) == 0) {
      // Found the user we want to deregister
      userToDeregister = curr;
      break;
    }
    prevUser = curr; // Keep track of the previous user for linked list removal
    curr = curr->next;
  }

  // If the user was not found, print an error message and stop
  if (userToDeregister == nullptr) {
    cout << "User " << username << " does not exist." << endl;
    return;
  }


  // Step 2: Find the Ghost User
  
  // The "ghost user" is a special user that represents deleted/unowned content.
  // When we deregister a user, we replace their ownership/authorship with the ghost user.
  // The ghost user should always exist in the system (it's typically created at startup).
  
  User *ghostUser = ghost;


  // Step 3: Update All References to the User
  
  // We need to find all places where this user is referenced and update them to the ghost user.
  // This includes commits, pull requests, and branches.
  
  // Walk through every repository in the system
  for (int repoIdx = 0; repoIdx < repoManagement.numRepos; repoIdx++) {
    Repository *repo = repoManagement.repos[repoIdx];
    
    // Update all commits in the repository's main branch
    // The main branch is represented by the commits linked list in the repository itself
    Commit *commitCurr = repo->commits;
    while (commitCurr != nullptr) {
      // If this commit was authored by the user we're deregistering,
      // replace the author with the ghost user
      if (commitCurr->author == userToDeregister) {
        commitCurr->author = ghostUser;
      }
      commitCurr = commitCurr->next;
    }

    // Update all commits in each branch
    for (int branchIdx = 0; branchIdx < repo->numBranches; branchIdx++) {
      Branch *branch = repo->branches[branchIdx];
      
      // If the branch creator is the user we're deregistering, replace them with ghost user
      if (branch->creator == userToDeregister) {
        branch->creator = ghostUser;
      }
      
      // Update all commits in this branch
      Commit *branchCommitCurr = branch->head;
      while (branchCommitCurr != nullptr) {
        // If this commit was authored by the user we're deregistering,
        // replace the author with the ghost user
        if (branchCommitCurr->author == userToDeregister) {
          branchCommitCurr->author = ghostUser;
        }
        branchCommitCurr = branchCommitCurr->next;
      }
    }

    // Update all pull requests in the repository
    for (int prIdx = 0; prIdx < repo->numPrs; prIdx++) {
      PullRequest *pr = repo->prs[prIdx];
      
      // If the pull request author is the user we're deregistering,
      // replace them with the ghost user
      if (pr->author == userToDeregister) {
        pr->author = ghostUser;
      }
    }
  }


  // Step 4: Delete All Repositories Owned by the User
  
  // This user owns some repositories (stored in userToDeregister->repos).
  // We need to delete all of them from the repository management system.
  
  // For each repository owned by this user
  for (int repoIdx = 0; repoIdx < userToDeregister->numRepos; repoIdx++) {
    Repository *repoToDelete = userToDeregister->repos[repoIdx];
    
    // Find this repository in the global repository management array and remove it
    int globalRepoIdx = -1;
    for (int i = 0; i < repoManagement.numRepos; i++) {
      if (repoManagement.repos[i] == repoToDelete) {
        globalRepoIdx = i;
        break;
      }
    }

    if (globalRepoIdx != -1) {
      // Create a new array without this repository
      Repository **newReposArray = nullptr;
      if (repoManagement.numRepos > 1) {
        newReposArray = new Repository*[repoManagement.numRepos - 1];
        int newIdx = 0;
        
        // Copy all repositories except the one we're deleting
        for (int i = 0; i < repoManagement.numRepos; i++) {
          if (i == globalRepoIdx) {
            continue; // Skip the repository we're deleting
          }
          newReposArray[newIdx] = repoManagement.repos[i];
          newIdx++;
        }
      }

      // Free the old array and update with the new one
      delete[] repoManagement.repos;
      repoManagement.repos = newReposArray;
      repoManagement.numRepos--;
    }

    // Free all the memory associated with this repository
    // First, delete all commits in the repository
    Commit *commitCurr = repoToDelete->commits;
    while (commitCurr != nullptr) {
      Commit *nextCommit = commitCurr->next;
      delete commitCurr;
      commitCurr = nextCommit;
    }

    // Delete all branches and their commits
    for (int branchIdx = 0; branchIdx < repoToDelete->numBranches; branchIdx++) {
      Branch *branch = repoToDelete->branches[branchIdx];
      
      // Delete all commits in this branch
      Commit *branchCommitCurr = branch->head;
      while (branchCommitCurr != nullptr) {
        Commit *nextCommit = branchCommitCurr->next;
        delete branchCommitCurr;
        branchCommitCurr = nextCommit;
      }
      
      // Delete the branch itself
      delete branch;
    }

    // Free the branches array
    if (repoToDelete->numBranches > 0) {
      delete[] repoToDelete->branches;
    }

    // Delete all pull requests
    for (int prIdx = 0; prIdx < repoToDelete->numPrs; prIdx++) {
      delete repoToDelete->prs[prIdx];
    }

    // Free the pull requests array
    if (repoToDelete->numPrs > 0) {
      delete[] repoToDelete->prs;
    }

    // Delete all forks information
    if (repoToDelete->numForks > 0) {
      delete[] repoToDelete->forks;
    }

    // Finally, delete the repository itself
    delete repoToDelete;
  }


  // Step 5: Remove the User from the Linked List
  
  // Now we need to remove the user from the linked list of users.
  
  if (prevUser == nullptr) {
    // The user to deregister is at the head of the linked list
    userManagement.head = userToDeregister->next;
  } 
  else {
    // The user to deregister is somewhere in the middle or end of the list
    prevUser->next = userToDeregister->next;
  }


  // Step 6: Deallocate Memory for the User
  
  // Free the dynamic array of repositories owned by the user
  if (userToDeregister->numRepos > 0) {
    delete[] userToDeregister->repos;
  }

  // Free the user structure itself
  delete userToDeregister;


  // Step 7: Print Success Message
  
  cout << "User " << username << " has been deregistered." << endl;
}
