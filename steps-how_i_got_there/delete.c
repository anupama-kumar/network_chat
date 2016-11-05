


        for( iter_x = head2; iter_x != NULL ; iter_x = iter_x->next_votefor)
            {
    
                if (strcmp(iter_x->data_name,newCand->data_name) == 0) {
                    newCand->data_count = newCand->data_count +1;
                    return EXISTS;
                }
                else {
                    iter_x->next_votefor = newCand;
                    newCand->data_count = newCand->data_count +1;
                    return NEW;
                }
    
            }




// this wont check last node
for( iter_x = head2; iter_x != NULL ; iter_x = iter_x->next_votefor)
    {
        if (strcmp(iter_x->data_name,newCand->data_name) == 0) {
            newCand->data_count = newCand->data_count +1;
            return EXISTS;
         }
    }
iter_x->next_votefor = newCand;
newCand->data_count = newCand->data_count +1;
return NEW;




// how to check for exists
while (iter1 != NULL) {
    if (iter1->data_voterid == voterid) {
        sendto(sock,"EXISTS",6,0,(const struct sockaddr *)&from,fromlen);
        return;
    }
    iter1 = iter1->next_addvoter;
}





iter_x = head2;
while( head2 != NULL){
    
    if (strcmp(iter_x->data_name,newCand->data_name) == 0) {
        newCand->data_count = newCand->data_count +1;
        return EXISTS;
    }
    iter_x = iter_x->next_votefor;
    
    if (strcmp(iter_x->data_name,newCand->data_name) != 0) {
        newCand->data_count = newCand->data_count +1;
        return NEW;
    }
    iter_x = iter_x->next_votefor;
}



// this is what was replaced by the above while loop:


// another solution using while loops instead of for
else{
     iter_x = head2;
    
    while ( iter_x->next_votefor != NULL){
        
        if((strcmp(iter_x->data_name,newCand->data_name) == 0) {
            newCand->data_count = newCand->data_count +1;
            return EXISTS;
        }
        
    } // end of while
        

}








    //VOTE_COUNT
    
    //void count(char *cand_name){
    //    struct votefor *iter;
    //    printf("%s",cand_name);
    //    for (iter = head2; iter !=NULL; iter = iter->next_votefor){
    //        if (strcmp(iter->data_name,cand_name) == 0) {
    //            printf("%d\n",iter->data_count);
    //        }
    //    }
    //    return;
    //}
    //
    //void votecount(char temp_vc[]){
    //    
    //    count(temp_vc);
    //    }















