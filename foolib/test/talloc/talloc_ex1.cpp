/*
 * talloc_ex1.cpp
 *
 *  Created on: 2016. 8. 7.
 *      Author: root
 */



#include <stdio.h>
#include <talloc.h>
#include <string.h>

struct user {
	uid_t uid;
	char* username;
	size_t num_groups;
	char** groups;
};

void show_user(const struct user* user) {
	printf("user=%p, uid=%u, username=%s, num_groups=%ld\n",
			user, user->uid, user->username, user->num_groups);
	for (int i=0; i<user->num_groups; i++) {
		printf("\t%s\n", user->groups[i]);
	}
}

void test_user_talloc() {
	struct user* user;
	struct user* user_zero;

	// create anew top level context
	user = talloc(NULL, struct user);
	{
		printf("user=%p, uid=%u, username=%s, num_groups=%ld\n",
				user, user->uid, user->username, user->num_groups);
	}

	user->uid = 100;
	user->num_groups = 5;

	// make user the parent of following contexts
	user->username = talloc_strdup(user, "Test user");
	user->groups = talloc_array(user, char*, user->num_groups);
	for (int i=0; i<user->num_groups; i++) {
		user->groups[i] = talloc_asprintf(user->groups, "Test group #%d", i);
	}

	show_user(user);

	// we can achieve the same result with
	user_zero = talloc_zero(NULL, struct user);
	{
		printf("user=%p, uid=%u, username=%s, num_groups=%ld\n",
				user_zero, user_zero->uid, user_zero->username, user_zero->num_groups);
	}

	talloc_free(user);
	talloc_free(user_zero);
}


int main(int argc, char **argv) {
	test_user_talloc();

	char *str_a = talloc_strdup(NULL, "hello world");
	char *str_b = talloc_strdup(NULL, "hello world");
	str_a[5] = str_b[5] = '\0';

	// str_a(5): hello
	printf("str_a(%ld): %s\n", strlen(str_a), str_a);
	// str_b(5): hello
	printf("str_b(%ld): %s\n", strlen(str_b), str_b);

	char *app = talloc_strdup_append(str_a, ", hello");
	char *buf = talloc_strdup_append_buffer(str_b, ", hello");

	// app(12): hello, hello
	printf("app(%ld): %s\n", strlen(app), app);
	// buf(5): hello
	printf("buf(%ld): %s\n", strlen(buf), buf);

	// str_a(12): hello, hello
	printf("str_a(%ld): %s\n", strlen(str_a), str_a);
	// str_b(5): hello
	printf("str_b(%ld): %s\n", strlen(str_b), str_b);

	// str_a talloc size: 13
	printf("str_a talloc size: %ld\n", talloc_get_size(str_a));
	// str_b talloc size: 19
	printf("str_b talloc size: %ld\n", talloc_get_size(str_b));

	// str_a talloc total size: 13
	printf("str_a talloc total size: %ld\n", talloc_total_size(str_a));
	// str_b talloc total size: 19
	printf("str_b talloc total size: %ld\n", talloc_total_size(str_b));


	str_b[5] = ' ';
	printf("str_b[5] = ' ' : %s\n", str_b);

	talloc_free(str_a);
	talloc_free(str_b);
}

