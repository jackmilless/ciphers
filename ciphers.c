#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include "ciphers.h"

void atbash(char *msg) {
	int len = strlen(msg);
	for (int i = 0; i < len; i++)
		msg[i] = 'z' - msg[i] + 'a';
	return;
}

void caesar(int key, char *msg) {
	char key_word[2];
	key_word[0] = 'a' + key;
	key_word[1] = '\0';
	vigenere(key_word, msg);
	return;
}

void decaesar(int key, char *msg) {
	int reverse_key = 26 - key;
	caesar(reverse_key, msg);
	return;
}

void vigenere(char *key, char *msg) {
	int offset = 0;
	int cur_m = msg[0];
	char cur_k = key[0];
	int len = strlen(key);
	int msg_len = strlen(msg);
	for (int i = 0; i < msg_len; i++) {
		cur_k = key[i % len];
		cur_m = msg[i];
		cur_m += cur_k - 'a';
		if (cur_m > 'z')
			cur_m -= 26;
		msg[i] = cur_m;
	}
	return;
}

void devigenere(char *key, char *msg) {
	int key_len = strlen(key);
	char key_word[key_len + 1];
	for (int i = 0; i < key_len; i++)
		key_word[i] = 'z' + 1 - key[i] + 'a';
	key_word[key_len] = '\0';
	vigenere(key_word, msg);
	return;
}

// helper function for various ciphers; determine if cur_letter is used yet
int is_used(char *used_letters, char cur_letter) {
	int used_len = strlen(used_letters);
	for (int i = 0; i < used_len; i++)
		if (used_letters[i] == cur_letter)
			return 1;
	return 0;
}

// helper function for several ciphers; fill a 5 x 5 polybius square using key
void polybiusize(char *key, char square[5][5]) {
	int key_len = strlen(key);
	int key_iter = 0;
	char used_letters[27]; // tracks letters already placed in square
	for (int i = 0; i < 26; i++) 
		used_letters[i] = ' '; 
	used_letters[26] = '\0';
	int used_iter = 0;
	char remaining_letters[27]; // letters unused in square
	int remain_iter = 0;
	char cur_key = key[0];
	// create polybius square
	for (int i = 0; i < 5; i++) {
		for (int j = 0; j < 5; j++) {
			// find next unique letter in key
			while (is_used(used_letters, cur_key) && key_iter < key_len) {
				cur_key = key[++key_iter];
				if (cur_key == 'j') // treat j as i
					cur_key = 'i';
			}
			// add next unique letter to square 
			if (key_iter < key_len) {
				square[i][j] = cur_key;
				used_letters[used_iter++] = cur_key;
				cur_key = key[++key_iter];
				if (cur_key == 'j')
					cur_key = 'i';
			} else { 
				if (key_iter == key_len) { // fill remaining_letters
					for (char c = 'a'; c <= 'z'; c++) {
						if (c != 'j') {
							while ((is_used(used_letters, c) && c <= 'z') || c == 'j')
								c++;
							remaining_letters[remain_iter++] = c;
						}
					}
					remain_iter = 0;
					key_iter++;
				}
				square[i][j] = remaining_letters[remain_iter++]; // add next remaining_letter to the square
			}
		}
	}
	return;
}

void polybius_square(char *key, char *msg) {
	char square[5][5]; 
	polybiusize(key, square); // create polybius square
	int msg_len = strlen(msg);
	char tmp[msg_len * 2 + 1];
	tmp[msg_len * 2] = '\0';
	char cur_letter;
	// convert each letter to its indices in polybius square
	for (int i = 0; i < msg_len; i++) {
		for (int j = 0; j < 5; j++) {
			for (int k = 0; k < 5; k++) {
				cur_letter = msg[i];
				if (cur_letter == 'j') // treat j as i
					cur_letter = 'i';
				if (cur_letter == square[j][k]) {
					tmp[i * 2] = j + '1';
					tmp[i * 2 + 1] = k + '1';
				}
			}
		}
	}
	strcpy(msg, tmp);
	return;
}

void depolybius(char *key, char *msg) {
	char square[5][5]; 
	polybiusize(key, square); // create polybius square
	int msg_len = strlen(msg);
	int row, col;
	// convert indices to letters in polybius square
	for (int i = 0; i < msg_len; i += 2) {
		row = msg[i] - '1';
		col = msg[i + 1] - '1';
		msg[i / 2] = square[row][col];
	}
	msg[msg_len / 2] = '\0';
	return;
}

void foursquare(char *key_one, char *key_two, char *msg) {
	// create 4 polybius squares, 2 of which are just the alphabet minus j
	char upper_left[5][5];
	char upper_right[5][5];
	char lower_left[5][5];
	char lower_right[5][5];
	polybiusize("abc", upper_left);
	polybiusize(key_one, upper_right);
	polybiusize(key_two, lower_left);
	polybiusize("abc", lower_right);

	int msg_len = strlen(msg);
	char letters[2];
	int rows[2]; 
	int cols[2];
	// encode pairs of letters, ignoring last one if odd
	for (int i = 0; i < msg_len - 1; i += 2) {
		letters[0] = msg[i];
		letters[1] = msg[i + 1];
		if (letters[0] == 'j')
			letters[0] = 'i';
		if (letters[1] == 'j')
			letters[1] = 'i';
		// find location of first letter in upper left square
		for (int m = 0; m < 5; m++) {
			for (int n = 0; n < 5; n++) {
				if (letters[0] == upper_left[m][n]) {
					rows[0] = m;
					cols[0] = n;
				}
			}
		}
		// find location of second letter in lower right square
		for (int m = 0; m < 5; m++) {
			for (int n = 0; n < 5; n++) {
				if (letters[1] == lower_right[m][n]) {
					rows[1] = m;
					cols[1] = n;
				}
			}
		}
		// encode letters in other two squares
		msg[i] = upper_right[rows[0]][cols[1]];
		msg[i + 1] = lower_left[rows[1]][cols[0]];
	}
	return;
}

void defoursquare(char *key_one, char *key_two, char *msg) {
	// create 4 polybius squares, 2 of which are just the alphabet minus j
	char upper_left[5][5];
	char upper_right[5][5];
	char lower_left[5][5];
	char lower_right[5][5];
	polybiusize("abc", upper_left);
	polybiusize(key_one, upper_right);
	polybiusize(key_two, lower_left);
	polybiusize("abc", lower_right);

	int msg_len = strlen(msg);
	char letters[2];
	int rows[2];
	int cols[2];
	// decode pairs of letters, ignoring last one if odd
	for (int i = 0; i < msg_len - 1; i += 2) {
		letters[0] = msg[i];
		letters[1] = msg[i + 1];
		if (letters[0] == 'j') // treat j as i
			letters[0] = 'i';
		if (letters[1] == 'j')
			letters[1] = 'i';
		// find location of first letter in upper right square
		for (int m = 0; m < 5; m++) {
			for (int n = 0; n < 5; n++) {
				if (letters[0] == upper_right[m][n]) {
					rows[0] = m;
					cols[0] = n;
				}
			}
		}
		// find location of second letter in lower left square
		for (int m = 0; m < 5; m++) {
			for (int n = 0; n < 5; n++) {
				if (letters[1] == lower_left[m][n]) {
					rows[1] = m;
					cols[1] = n;
				}
			}
		}
		// find original letters of message in other two squares
		msg[i] = upper_left[rows[0]][cols[1]];
		msg[i + 1] = lower_right[rows[1]][cols[0]];
	}
	return;
}

void playfair(char *key, char *msg) {
	char square[5][5];
	polybiusize(key, square); // create polybius square
	int msg_len = strlen(msg);
	char tmp[msg_len + 1];
	int tmp_iter = 0;
	int letters_pair[2];
	int step, is_last_letter;
	// encode pairs of letters
	for (int i = 0; i < msg_len; i += step) {
		is_last_letter = (i == msg_len - 1); 
		letters_pair[0] = msg[i];
		if (letters_pair[0] == 'j') // treat j as i
			letters_pair[0] = 'i';
		if (!is_last_letter) { 
			letters_pair[1] = msg[i + 1];
			if (letters_pair[1] == 'j')
				letters_pair[1] = 'i';
		}
		if (is_last_letter || (letters_pair[0] == letters_pair[1])) { // last letter or letters are the same
			letters_pair[1] = 'x';
			step = 1;
		}
		else
			step = 2;
		// find positions of both letters in square
		int rows[2], cols[2];
		for (int m = 0; m < 5; m++) {
			for (int n = 0; n < 5; n++) {
				if (square[m][n] == letters_pair[0]) {
					rows[0] = m;
					cols[0] = n;
				}
				if (square[m][n] == letters_pair[1]) {
					rows[1] = m;
					cols[1] = n;
				}
			}
		}
		// encode message with square, depending on case
		if (cols[0] == cols[1]) { // same column 
			tmp[tmp_iter++] = square[(rows[0] + 1) % 5][cols[0]];
			tmp[tmp_iter++] = square[(rows[1] + 1) % 5][cols[0]];
		} else if (rows[0] == rows[1]) { // same row
			tmp[tmp_iter++] = square[rows[0]][(cols[0] + 1) % 5];
			tmp[tmp_iter++] = square[rows[0]][(cols[1] + 1) % 5];
		} else { // corners
			tmp[tmp_iter++] = square[rows[0]][cols[1]];
			tmp[tmp_iter++] = square[rows[1]][cols[0]];
		}
	}
	tmp[tmp_iter] = '\0';
	strcpy(msg, tmp);
	return;
}

void deplayfair(char *key, char *msg) {
	char square[5][5];
	polybiusize(key, square);  // create polybius square
	int msg_len = strlen(msg);
	char tmp[msg_len + 1];
	int tmp_iter = 0;
	int letters_pair[2];
	// decode pairs of letters
	for (int i = 0; i < msg_len; i += 2) {
		letters_pair[0] = msg[i];
		letters_pair[1] = msg[i + 1];
		if (letters_pair[0] == 'j') // treat j as i
			letters_pair[0] = 'i';
		if (letters_pair[1] == 'j')
			letters_pair[1] = 'i';
		// find positions of both letters in square
		int rows[2], cols[2];
		for (int m = 0; m < 5; m++) {
			for (int n = 0; n < 5; n++) {
				if (square[m][n] == letters_pair[0]) {
					rows[0] = m;
					cols[0] = n;
				}
				if (square[m][n] == letters_pair[1]) {
					rows[1] = m;
					cols[1] = n;
				}
			}
		}
		// decode message with square, depending on case
		int square_idx;
		if (cols[0] == cols[1]) { // same column
			square_idx = (rows[0] - 1 < 0) ? rows[0] + 4 : rows[0] - 1;
			tmp[tmp_iter++] = square[square_idx % 5][cols[0]];
			square_idx = (rows[1] - 1 < 0) ? rows[1] + 4 : rows[1] - 1;
			tmp[tmp_iter++] = square[square_idx % 5][cols[0]];
		} else if (rows[0] == rows[1]) { // same row 
			square_idx = (cols[0] - 1 < 0) ? cols[0] + 4 : cols[0] - 1;
			tmp[tmp_iter++] = square[rows[0]][square_idx % 5];
			square_idx = (cols[1] - 1 < 0) ? cols[1] + 4 : cols[1] - 1;
			tmp[tmp_iter++] = square[rows[0]][square_idx % 5];
		} else { // corners 
			tmp[tmp_iter++] = square[rows[0]][cols[1]];
			tmp[tmp_iter++] = square[rows[1]][cols[0]];
		}
	}
	tmp[tmp_iter] = '\0';
	strcpy(msg, tmp);
	return;
}

// helper function for straddling_checkerboard and columnar_transposition; 
// determines if index in visited (int or char array) has been visited yet
int is_visited(int *visited, int index) {
	for (int i = 0; visited[i] != -1; i++)
		if (visited[i] == index)
			return 1;
	return 0;
}

// helper function for merge_sort
void merge(char *arr, int left, int mid, int right) {
	char barr[right - left + 2];
	int left_count = left;
	int right_count = mid + 1;
	int i = 0;

	while ((left_count <= mid) && (right_count <= right)) {
		if (arr[left_count] <= arr[right_count])
			barr[i++] = arr[left_count++];
		else
			barr[i++] = arr[right_count++];
	}

	if (left_count > mid) {
		while (right_count <= right)
			barr[i++] = arr[right_count++];
	} else {
		while (left_count <= mid)
			barr[i++] = arr[left_count++];
	}

	for (int j = 0; j < right - left + 1; j++)
		arr[left + j] = barr[j];
	return;
}

// helper function for straddling_checkerboard and columnar_transposition
void merge_sort(char *arr, int left, int right) {
	int mid;
	if (left < right) {
		mid = (left + right) / 2;
		merge_sort(arr, left, mid);
		merge_sort(arr, mid + 1, right);
		merge(arr, left, mid, right);
	}
	return;
}

void scytale(int height, char *msg) {
	int len = strlen(msg);
	char tmp[len + 1];
	int num_chars_on_row[height]; 
	for (int i = 0; i < height; i++)
		num_chars_on_row[i] = ceil(((double)len - i) / height);
	int offset = 0;
	int msg_iter = 0;
	// sets of stairs
	for (int i = 0; i < ceil((double)len / height); i++) {
		// current set of stairs
		for (int j = offset; j < len; j += num_chars_on_row[(msg_iter - 1) % height]) 
			if (msg_iter < len)
				tmp[msg_iter++] = msg[j];
		offset++;
	}
	tmp[len] = '\0';
	strcpy(msg, tmp);
	return;
}

void descytale(int height, char *msg) {
	int len = strlen(msg);
	char tmp[len + 1];
	tmp[len] = '\0';
	int tmp_iter = 0;
	for (int i = 0; i < height; i++) // sets of stairs
		for (int j = i; j < len; j += height) // current set of stairs
			tmp[tmp_iter++] = msg[j];
	strcpy(msg, tmp);
	return;
}

void railfence(int height, int direction, char *msg) {
	int msg_len = strlen(msg);
	int start_pos = height - (height * direction); // 0 if direction 1, height if direction 0
	int shift = (direction << 1) - 1; // -1 if direction 0,  1 if direction 1 
	int max_gap = 2 * height;
	int cur_gap = max_gap * direction; // distance between letters on rail
	char tmp[msg_len + 1];
	strcpy(tmp, msg);
	int k = 0;
	for (int i = 0; i <= height; i++) { // rails
		int copy_gap = cur_gap;
		if (cur_gap == max_gap) // when on uppermost or lowermost rail, don't alternate
			cur_gap = 0;
		for (int j = start_pos; j < msg_len; j += max_gap - cur_gap) { // current rail
			msg[k] = tmp[j];
			k++;
			if (cur_gap != 0)
				cur_gap = max_gap - cur_gap; // gap usually alternates along rail
		}
		cur_gap = copy_gap;
		start_pos += shift;
		cur_gap -= 2 * shift;
	}
	return;
}

void derailfence(int height, int direction, char *msg) {
	// determine number of characters in outer (lowest and highest) rails
	int msg_len = strlen(msg);
	int max_gap = height * 2;
	int num_chars_per_rail[height + 1]; 
	num_chars_per_rail[0] = ceil((double)msg_len / max_gap);
	num_chars_per_rail[height] = ceil((double)(msg_len - height) / max_gap);
	if (!direction) { // values flipped if starting from bottom
		int tmp = num_chars_per_rail[0];
		num_chars_per_rail[0] = num_chars_per_rail[height];
		num_chars_per_rail[height] = tmp;
	}
	int cur_gap = max_gap; // distance between letters on rail
	int max_chars_in_rail = 
		(num_chars_per_rail[0] > num_chars_per_rail[height]) ? num_chars_per_rail[0]
															 : num_chars_per_rail[height];
	int cur_direction = (direction << 1) - 1; // 1 if direction 1, -1 if direction 0
	// determine number of characters in inner rails.
	for (int i = 1; i < height; i++) { // inner rails
		int pos = i;
		int cur_rail_size = 0;
		cur_gap -= 2;			   
		int copy_gap = cur_gap;
		while (pos < msg_len) {
			cur_rail_size++;
			pos += copy_gap;						
			copy_gap = max_gap - copy_gap; // alternate as in encoding
		}
		if (cur_rail_size > max_chars_in_rail)
			max_chars_in_rail = cur_rail_size;
		num_chars_per_rail[(1 - direction) * height + (i * cur_direction)] = cur_rail_size;
	}
	char rails[height + 2][max_chars_in_rail + 1]; 
	int msg_iter = 0;
	// store original msg in rail structure
	for (int i = 0; i < height + 1; i++) { 
		for (int j = 0; j < num_chars_per_rail[i]; j++) {
			rails[i][j] = msg[msg_iter];
			msg_iter++;
		}
	}
	char tmp[msg_len + 1];
	int rail_iters[height + 1]; // number of characters collected for each rail so far
	for (int i = 0; i < height + 1; i++)
		rail_iters[i] = 0;
	int cur_height = (1 - direction) * height; // 0 if direction 1, height if direction 0
	for (int i = 0; i < msg_len; i++) {  
		tmp[i] = rails[cur_height][rail_iters[cur_height]];
		rail_iters[cur_height]++;
		cur_height += cur_direction;
		if ((cur_height == 0) || (cur_height == height))
			cur_direction *= -1;
	}
	tmp[msg_len] = '\0';
	strcpy(msg, tmp);
	return;
}

void spiral(int rows, int cols, int layout_direction, int corner, int clockwise, char *msg) {
	char grid[rows + 1][cols + 1];
	int max_col = cols - 1;
	int max_row = rows - 1;

	int col_direction = layout_direction & 1; // 0 if left to right, 1 if right to left
	int col_start = max_col * col_direction; 
	int col_end = max_col - col_start;
	col_direction = -1 * ((col_direction << 1) - 1); // 1 if left to r, -1 if right to l

	int row_direction = layout_direction >> 1; // 0 if top to bottom, 1 if bottom to top
	int row_start = max_row * row_direction; 
	int row_end = max_row - row_start;
	row_direction = -1 * ((row_direction << 1) - 1); // 1 if top to bottom, -1 if bottom to top
	int offset = 0;
	// fill in grid
	for (int j = col_start; j != col_end + col_direction; j += col_direction) {
		for (int i = row_start; i != row_end + row_direction; i += row_direction)
			grid[i][j] = msg[offset + (row_direction * i) + row_start];
		offset += rows;
	}
	int circular_direction = (clockwise << 1) - 1; // -1 if ccw, 1 if cw
	int directions[4][2] = {{0, 1}, {1, 1}, {0, -1}, {1, -1}}; // {0: vertical or 1: horizontal, -1: negative or 1: positive}
	int corners[4][2] = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
	int boundaries[4] = {0, 0, 0, 0}; // track boundaries as outer, used grid becomes larger

	int cur_direction = (corner + (1 - clockwise)) & 0b11; // & 0b11 is the same as % 4
	int cur_corner = corner;
	int cur_row = corners[cur_corner][0];
	int cur_col = corners[cur_corner][1];
	int horizontal = directions[cur_direction][0];
	int forward = directions[cur_direction][1];

	int len = strlen(msg);
	int msg_iter = 0;
	int start_row, start_col, row_bound, col_bound, opposite_boundary;
	int row_or_col_const, row_or_col_start, row_or_col_end, max_row_or_col;
	// spiral around grid until all letters reached
	while (msg_iter < len) {
		if (horizontal) { // boundaries if moving left or right
			row_bound = boundaries[(cur_direction - circular_direction) & 0b11];
			col_bound = boundaries[cur_direction];
		} else { // boundaries if moving up or down
			row_bound = boundaries[cur_direction];
			col_bound = boundaries[(cur_direction - circular_direction) & 0b11];
		}
		start_row = cur_row * max_row + (-1 * ((cur_row << 1) - 1)) * row_bound;
		start_col = cur_col * max_col + (-1 * ((cur_col << 1) - 1)) * col_bound;
		row_or_col_const = (horizontal == 0) ? start_row : start_col;
		row_or_col_start = (horizontal == 1) ? start_row : start_col;
		max_row_or_col = horizontal ? max_row : max_col;
		opposite_boundary = boundaries[(cur_direction + circular_direction) & 0b11];
		row_or_col_end = (forward == 1) ? max_row_or_col - opposite_boundary : opposite_boundary;
		for (int i = row_or_col_start; i != row_or_col_end + forward; i += forward)
			msg[msg_iter++] = horizontal ? grid[i][row_or_col_const]
										 : grid[row_or_col_const][i];
		boundaries[cur_direction]++;
		cur_direction = (cur_direction + circular_direction) & 0b11;
		horizontal = directions[cur_direction][0];
		forward = directions[cur_direction][1];
		cur_corner = (cur_corner + circular_direction) & 0b11;
		cur_row = corners[cur_corner][0];
		cur_col = corners[cur_corner][1];
	}
	return;
}

void despiral(int rows, int cols, int layout_direction, int corner, int clockwise, char *msg) {
	char grid[rows + 1][cols + 1];
	int max_col = cols - 1;
	int max_row = rows - 1;

	int circular_direction = (clockwise << 1) - 1; // -1 if ccw, 1 if cw
	int directions[4][2] = {{0, 1}, {1, 1}, {0, -1}, {1, -1}}; // {0: vertical or 1: horizontal, -1: negative or 1: positive}
	int corners[4][2] = {{0, 0}, {0, 1}, {1, 1}, {1, 0}};
	int boundaries[4] = {0, 0, 0, 0}; // track boundaries as outer, used grid becomes larger

	int cur_direction = (corner + (1 - clockwise)) & 0b11; // & 0b11 is the same as % 4
	int cur_corner = corner;
	int cur_row = corners[cur_corner][0];
	int cur_col = corners[cur_corner][1];
	int horizontal = directions[cur_direction][0];
	int forward = directions[cur_direction][1];

	int len = strlen(msg);
	int msg_iter = 0;
	int start_row, start_col, row_bound, col_bound, opposite_boundary;
	int row_or_col_const, row_or_col_start, row_or_col_end, max_row_or_col;
	// spiral around grid to fill it in
	while (msg_iter < len) {
		if (horizontal) { // boundaries if moving left or right
			row_bound = boundaries[(cur_direction - circular_direction) & 0b11];
			col_bound = boundaries[cur_direction];
		} else { // boundaries if moving up or down
			row_bound = boundaries[cur_direction];
			col_bound = boundaries[(cur_direction - circular_direction) & 0b11];
		}
		start_row = cur_row * max_row + (-1 * ((cur_row << 1) - 1)) * row_bound;
		start_col = cur_col * max_col + (-1 * ((cur_col << 1) - 1)) * col_bound;
		row_or_col_const = (horizontal == 0) ? start_row : start_col;
		row_or_col_start = (horizontal == 1) ? start_row : start_col;
		max_row_or_col = horizontal ? max_row : max_col;
		opposite_boundary = boundaries[(cur_direction + circular_direction) & 0b11];
		row_or_col_end = (forward == 1) ? max_row_or_col - opposite_boundary : opposite_boundary;
		for (int i = row_or_col_start; i != row_or_col_end + forward; i += forward) {
			if (horizontal)
				grid[i][row_or_col_const] = msg[msg_iter++];
			else
				grid[row_or_col_const][i] = msg[msg_iter++];
		}
		boundaries[cur_direction]++;
		cur_direction = (cur_direction + circular_direction) & 0b11;
		horizontal = directions[cur_direction][0];
		forward = directions[cur_direction][1];
		cur_corner = (cur_corner + circular_direction) & 0b11;
		cur_row = corners[cur_corner][0];
		cur_col = corners[cur_corner][1];
	}
	int col_direction = layout_direction & 1; // 0 if left to right, 1 if right to left
	int col_start = max_col * col_direction; 
	int col_end = max_col - col_start;
	col_direction = -1 * ((col_direction << 1) - 1); // 1 if left to right, -1 if right to lleft

	int row_direction = layout_direction >> 1; // 0 if top to bottom, 1 if bottom to top
	int row_start = max_row * row_direction; 
	int row_end = max_row - row_start;
	row_direction = -1 * ((row_direction << 1) - 1); // 1 if top to bottom, -1 if bottom to top

	// read original message from grid
	int offset = 0;
	for (int j = col_start; j != col_end + col_direction; j += col_direction) {
		for (int i = row_start; i != row_end + row_direction; i += row_direction)
			msg[offset + (row_direction * i) + row_start] = grid[i][j];
		offset += rows;
	}
}

// helper struct for columnar_transposition
typedef struct {
	char letter; 
	int num; // number of instances of letter in a key
	int *indices; // pointer to num indices associated with letter
} letters_t;

void columnar_transposition(char *keys, int num_transpositions,
							int is_myszkowski, char *disruption_key, char *msg) {
	int disruption_key_len = strlen(disruption_key);
	int is_disrupted = disruption_key_len - 1; // not disrupted if disruption_key_len == 1
	int gaps[disruption_key_len];
	if (is_disrupted) {
		// determine order of "gaps" between spaces
		char copy_disruption_key[disruption_key_len + 1];
		strcpy(copy_disruption_key, disruption_key);
		merge_sort(copy_disruption_key, 0, (disruption_key_len - 1));
		char disruption_letter;
		int disruption_letters_visited[disruption_key_len + 1];
		disruption_letters_visited[disruption_key_len] = -1; 
		for (int i = 0; i < disruption_key_len; i++)
			disruption_letters_visited[i] = -2;
		int disruption_iterator = 0;
		for (int i = 0; i < disruption_key_len; i++) {
			disruption_letter = copy_disruption_key[i];
			for (int j = 0; j < disruption_key_len; j++) {
				if (disruption_letter == disruption_key[j] && !is_visited(disruption_letters_visited, j)) {
					gaps[j] = disruption_iterator;
					disruption_letters_visited[disruption_iterator] = j;
					disruption_iterator++;
					break;
				}
			}
		}
	}
	int total_key_len = strlen(keys);
	int char_pos = 0;
	int msg_len = strlen(msg);
	char cur_char = keys[0];

	for (int a = 0; a < num_transpositions; a++) {
		// determine length of cur_key, updating char_pos for next iteration of loop
		int key_len = 0;
		char_pos %= total_key_len;
		int tmp_pos = char_pos;
		while ((cur_char != ' ') && (tmp_pos < total_key_len)) {
			cur_char = keys[++tmp_pos];
			key_len++;
		}
		char cur_key[key_len + 1];
		for (int i = 0; i < key_len; i++)
			cur_key[i] = keys[char_pos++]; 
		cur_key[key_len] = '\0';
		while (cur_char == ' ') 
			cur_char = keys[++char_pos];

		int num_grid_rows;
		int total_grid_size = msg_len;
		if (is_disrupted) { // adjust total_grid_size to account for gaps
			int gap_iter = 0;
			int i = 0;
			while (i < msg_len) {
				for (int j = 0; j < gaps[gap_iter]; j++)
					i++;
				total_grid_size++;
				gap_iter = (gap_iter + 1) % disruption_key_len;
			}
		}
		num_grid_rows = ceil((double)total_grid_size / key_len);
		char grid[num_grid_rows + 1][key_len + 1];
		// fill grid
		if (!is_disrupted) { // fill grid without spaces
			for (int i = 0; i < num_grid_rows - 1; i++)
				for (int j = 0; j < key_len; j++)
					grid[i][j] = msg[i * key_len + j];
			int msg_idx;
			for (int j = 0; j < key_len; j++) { // last row
				msg_idx = (num_grid_rows - 1) * key_len + j;
				grid[num_grid_rows - 1][j] = (msg_idx < msg_len) ? msg[msg_idx] : '\0';
			}
		} else { // fill grid with spaces
			int gap_iter = 0;
			int cur_iter = 0;
			int msg_iter = 0;
			int max_cur = gaps[0];
			for (int i = 0; i < num_grid_rows; i++) {
				for (int j = 0; j < key_len; j++) {
					if (cur_iter < max_cur) { // fill normally until gap number is hit
						grid[i][j] = (msg_iter < msg_len) ? msg[msg_iter] : '\0';
						msg_iter++;
						cur_iter++;
					} else { // once gap number hit, enter a space and move to next gap
						grid[i][j] = ' ';
						cur_iter = 0;
						gap_iter = (gap_iter + 1) % disruption_key_len;
						max_cur = gaps[gap_iter];
					}
				}
			}
		}
		char copy_cur_key[key_len + 1];
		strcpy(copy_cur_key, cur_key);
		merge_sort(copy_cur_key, 0, (key_len - 1));
		char cur_letter;
		int visited_indices_iter = 0;
		int msg_iter = 0;
		char tmp[total_grid_size + 1];
		// encode message (primarily) along columns
		if (!is_myszkowski) { // normal encoding with repeat letters
			int visited_indices[key_len + 1];
			visited_indices[key_len] = -1;
			for (int i = 0; i < key_len; i++)
				visited_indices[i] = -2;
			for (int i = 0; i < key_len; i++) {
				cur_letter = copy_cur_key[i];
				for (int j = 0; j < key_len; j++) {
					if (cur_letter == cur_key[j] && !is_visited(visited_indices, j)) {
						for (int k = 0; k < num_grid_rows; k++)
							if (grid[k][j] != '\0' && grid[k][j] != ' ') // ignore spaces
								tmp[msg_iter++] = grid[k][j];
						visited_indices[visited_indices_iter] = j;
						visited_indices_iter++;
						break;
					}
				}
			}
		} else { // encode horizontally first in case of repeat letters 
			int num_distinct_letters = 1; // number of unique letters in key
			for (int i = 0; i < key_len - 1; i++) {
				if (copy_cur_key[i + 1] != copy_cur_key[i])
					num_distinct_letters++;
			}
			letters_t letters[num_distinct_letters]; 
			int letter_iter = 0;
			int num_cur_letter = 1;
			int all_key_indices[key_len]; // saves indices after for loop
			int offset = 0;
			// fill letters structs
			for (int i = 0; i < key_len; i++) {
				cur_letter = copy_cur_key[i];
				if ((copy_cur_key[i + 1] == cur_letter) && (i < key_len - 1))
					num_cur_letter++;
				else {
					int *key_indices = &all_key_indices[offset];
					letters[letter_iter].letter = cur_letter;
					letters[letter_iter].num = num_cur_letter;
					letters[letter_iter].indices = key_indices;
					offset += num_cur_letter;
					letter_iter++;
					num_cur_letter = 1;
				}
			}
			letter_iter = 0;
			for (int i = 0; i < num_distinct_letters; i++) {
				cur_letter = letters[i].letter;
				for (int j = 0; j < key_len; j++) {
					if (cur_letter == cur_key[j]) {
						letters[i].indices[letter_iter] = j;
						letter_iter++;
					}
				}
				letter_iter = 0;
			}
			// transcribe values across rows of the same letter, then down those columns
			int cur_col;
			int cur_num;
			for (int i = 0; i < num_distinct_letters; i++) {
				for (int k = 0; k < num_grid_rows; k++) {
					cur_num = letters[i].num;
					for (int j = 0; j < cur_num; j++) {
						cur_col = letters[i].indices[j];
						if (grid[k][cur_col] != '\0' && grid[k][cur_col] != ' ') // ignore spaces
							tmp[msg_iter++] = grid[k][cur_col];
					}
				}
			}
		}
		tmp[msg_iter] = '\0';
		strcpy(msg, tmp);
	}
	return;
}

void decolumnar(char *keys, int num_transpositions, int is_myszkowski,
				char *disruption_key, char *msg) {
	int disruption_key_len = strlen(disruption_key);
	int is_disrupted = disruption_key_len - 1; // not disrupted if disruption_key_len == 1
	int gaps[disruption_key_len];
	if (is_disrupted) {
		// determine order of "gaps" between spaces
		char copy_disruption_key[disruption_key_len + 1];
		strcpy(copy_disruption_key, disruption_key);
		merge_sort(copy_disruption_key, 0, (disruption_key_len - 1));
		char disruption_letter;
		int disruption_letters_visited[disruption_key_len + 1];
		disruption_letters_visited[disruption_key_len] = -1;
		for (int i = 0; i < disruption_key_len; i++)
			disruption_letters_visited[i] = -2;
		int disruption_iterator = 0;
		for (int i = 0; i < disruption_key_len; i++) {
			disruption_letter = copy_disruption_key[i];
			for (int j = 0; j < disruption_key_len; j++) {
				if (disruption_letter == disruption_key[j] && !is_visited(disruption_letters_visited, j)) {
					gaps[j] = disruption_iterator;
					disruption_letters_visited[disruption_iterator] = j;
					disruption_iterator++;
					break;
				}
			}
		}
	}
	// determine number of keys and space needed to store them
	int spaceless_keys_len = 0;
	int num_keys = 1;
	int total_keys_len = strlen(keys);
	char cur_char;
	for (int i = 0; i < total_keys_len; i++) {
		cur_char = keys[i];
		if (cur_char == ' ') {
			num_keys++;
			while ((cur_char == ' ') && i < total_keys_len - 1)
				cur_char = keys[++i];
			spaceless_keys_len += 2;
		}
		else
			spaceless_keys_len++;
	}
	// find all keys and store them
	int cur_key_len = 0;
	char key_space[spaceless_keys_len + 1];
	char *keys_array[num_keys];
	int offset = 0;
	int keys_iter = 0;
	int copy_keys_iter;
	cur_char = keys[0];
	for (int i = 0; i < num_keys; i++) {
		cur_char = keys[keys_iter];
		copy_keys_iter = keys_iter;
		while ((cur_char != ' ') && keys_iter < total_keys_len - 1) {
			cur_key_len++;
			cur_char = keys[++keys_iter];
		}
		if (cur_char != ' ' && keys_iter == total_keys_len - 1)
			cur_key_len++;
		while ((cur_char == ' ') && keys_iter < total_keys_len - 1)
			cur_char = keys[++keys_iter];
		keys_array[i] = &key_space[offset];
		for (int j = 0; j < cur_key_len; j++)
			keys_array[i][j] = keys[copy_keys_iter + j];
		keys_array[i][cur_key_len] = '\0';
		offset += cur_key_len + 1;
		cur_key_len = 0;
	}
	// note where keys start for decoding
	int msg_len = strlen(msg);
	int cur_key_idx = (num_transpositions % num_keys) - 1;
	if (cur_key_idx < 0)
		cur_key_idx += num_keys;

	for (int a = 0; a < num_transpositions; a++) {
		int num_grid_rows;
		int total_grid_size = msg_len;
		if (is_disrupted) { // adjust total_grid_size to account for gaps
			int gap_iter = 0;
			int i = 0;
			while (i < msg_len) {
				for (int j = 0; j < gaps[gap_iter]; j++)
					i++;
				if (i < msg_len)
					total_grid_size++;
				gap_iter = (gap_iter + 1) % disruption_key_len;
			}
		}
		int key_len = strlen(keys_array[cur_key_idx]);
		num_grid_rows = ceil((double)total_grid_size / key_len);
		char grid[num_grid_rows + 1][key_len + 1];
		int msg_iter = 0;
		// initialize (not fill) grid
		if (is_disrupted) { // add spaces to grid
			int gap_iter = 0;
			int cur_iter = 0;
			int max_cur = gaps[0];
			for (int i = 0; i < num_grid_rows; i++) {
				for (int j = 0; j < key_len; j++) {
					if (cur_iter < max_cur) { // initialize to '1' if not gap number
						grid[i][j] = (msg_iter < msg_len) ? '1' : '\0';
						msg_iter++;
						cur_iter++;
					} else { // once gap number hit, enter a space and move to next gap
						grid[i][j] = ' ';
						cur_iter = 0;
						gap_iter = (gap_iter + 1) % disruption_key_len;
						max_cur = gaps[gap_iter];
					}
				}
			}
		} else { // initialize everything to '1'
			for (int i = 0; i < num_grid_rows; i++)
				for (int j = 0; j < key_len; j++)
					grid[i][j] = (msg_iter++ < msg_len) ? '1' : '\0';
		}
		for (int i = 0; i < key_len; i++)
			grid[num_grid_rows][i] = '\0';
		char cur_key[key_len + 1];
		strcpy(cur_key, keys_array[cur_key_idx]);
		char copy_key[key_len + 1];
		strcpy(copy_key, cur_key);
		merge_sort(copy_key, 0, (key_len - 1));
		char cur_letter;
		int visited_indices_iter = 0;
		char tmp[total_grid_size + 1];
		int column_order[key_len];
		int order_iter = 0;
		msg_iter = 0;
		// fill grid (primarily) along columns
		if (!is_myszkowski) { // normal grid filling with repeat letters
			int visited_indices[key_len + 1];
			visited_indices[key_len] = -1;
			for (int i = 0; i < key_len; i++)
				visited_indices[i] = -2;
			for (int i = 0; i < key_len; i++) {
				cur_letter = copy_key[i];
				for (int j = 0; j < key_len; j++) {
					if (cur_letter == cur_key[j] && !is_visited(visited_indices, j)) {
						for (int k = 0; k < num_grid_rows; k++)
							if (grid[k][j] != '\0' && grid[k][j] != ' ') // ignore spaces
								grid[k][j] = msg[msg_iter++];
						visited_indices[visited_indices_iter++] = j;
						break;
					}
				}
			}
		} else { // assign values to grid horizontally first in case of repeat letters 
			int num_distinct_letters = 1; // number of unique letters in key
			for (int i = 0; i < key_len - 1; i++) {
				if (copy_key[i + 1] != copy_key[i])
					num_distinct_letters++;
			}
			letters_t letters[num_distinct_letters]; // saves indices after for loop
			int letter_iter = 0;
			int num_cur_letter = 1;
			int all_key_indices[key_len];
			int offset = 0;
			// fill letters structs
			for (int i = 0; i < key_len; i++) {
				cur_letter = copy_key[i];
				if ((copy_key[i + 1] == cur_letter) && (i < key_len - 1))
					num_cur_letter++;
				else {
					int *key_indices = &all_key_indices[offset];
					letters[letter_iter].letter = cur_letter;
					letters[letter_iter].num = num_cur_letter;
					letters[letter_iter].indices = key_indices;
					offset += num_cur_letter;
					letter_iter++;
					num_cur_letter = 1;
				}
			}
			letter_iter = 0;
			for (int i = 0; i < num_distinct_letters; i++) {
				cur_letter = letters[i].letter;
				for (int j = 0; j < key_len; j++) {
					if (cur_letter == cur_key[j]) {
						letters[i].indices[letter_iter] = j;
						letter_iter++;
					}
				}
				letter_iter = 0;
			}
			// assign values to grid across rows of the same letter, then down those columns
			int cur_col, cur_num;
			for (int i = 0; i < num_distinct_letters; i++) {
				for (int k = 0; k < num_grid_rows; k++) {
					cur_num = letters[i].num;
					for (int j = 0; j < cur_num; j++) {
						cur_col = letters[i].indices[j];
						if (grid[k][cur_col] != '\0' && grid[k][cur_col] != ' ') // ignore spaces
							grid[k][cur_col] = msg[msg_iter++];
					}
				}
			}
		}
		// read left to right, top to bottom across decoded grid to get final message
		msg_iter = 0;
		for (int i = 0; i < num_grid_rows; i++)
			for (int j = 0; j < key_len; j++)
				if (grid[i][j] != ' ' && grid[i][j] != '\0')
					msg[msg_iter++] = grid[i][j];
		msg[msg_iter] = '\0';
		msg_iter = 0;
		// adjust key for next iteration
		cur_key_idx--;
		if (cur_key_idx < 0)
			cur_key_idx += num_keys;
	}
	return;
}

void bifid(char *key, char *msg) {
	polybius_square(key, msg); // encode with the polybius square cipher
	int msg_len = strlen(msg);
	char reoriented_digits[msg_len + 1];
	reoriented_digits[msg_len] = '\0';
	int msg_iter = 0;
	// re-orient digits of encoded message
	for (int i = 0; i < msg_len; i += 2)
		reoriented_digits[msg_iter++] = msg[i];
	for (int i = 1; i < msg_len; i += 2)
		reoriented_digits[msg_iter++] = msg[i];
	strcpy(msg, reoriented_digits);
	depolybius(key, msg); // re-encode digits as letters
	return;
}

void debifid(char *key, char *msg) {
	polybius_square(key, msg); // decode with the polybius square cipher
	int msg_len = strlen(msg);
	char reoriented_digits[msg_len + 1];
	reoriented_digits[msg_len] = '\0';
	int msg_iter = 0;
	int mid = msg_len / 2;
	// re-orient digits of still encoded message
	for (int i = 0; i < mid; i++) {
		reoriented_digits[i * 2] = msg[i];
		reoriented_digits[i * 2 + 1] = msg[mid + i];
	}
	strcpy(msg, reoriented_digits);
	depolybius(key, msg); // finish decoding by converting back to letters
	return;
}

void trifid(char *key, int group_size, char *msg) {
	char squares[3][3][3];
	int key_len = strlen(key);
	int key_iter = 0;
	char used_letters[28]; // tracks letters already placed in squares
	for (int i = 0; i < 27; i++)
		used_letters[i] = ' '; 
	used_letters[27] = '\0';
	int used_iter = 0;
	char remaining_letters[28]; // letters unused in squares (and "+")
	int remain_iter = 0;
	char cur_key = key[0];
	// fill in squares
	for (int l = 0; l < 3; l++) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				// find next unique letter in key
				while (is_used(used_letters, cur_key) && key_iter < key_len)
					cur_key = key[++key_iter];
				// add next unique letter to current square
				if (key_iter < key_len) {
					squares[l][i][j] = cur_key;
					used_letters[used_iter++] = cur_key;
					cur_key = key[++key_iter];
				} else {
					if (key_iter == key_len) { // fill remaining_letters
						for (char c = 'a'; c <= 'z'; c++) {
							while ((is_used(used_letters, c) && c <= 'z'))
								c++;
							remaining_letters[remain_iter++] = c;
						}
						remaining_letters[remain_iter] = '+';
						remain_iter = 0;
						key_iter++;
					}
					squares[l][i][j] = remaining_letters[remain_iter++]; // add next remaining_letter to the current square
				}
			}
		}
	}
	int msg_len = strlen(msg);
	int tmp_len = msg_len * 3;
	char tmp[tmp_len + 1];
	tmp[tmp_len] = '\0';
	char cur_letter;
	// convert each letter to its indices in squares
	for (int a = 0; a < msg_len; a++) {
		for (int l = 0; l < 3; l++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					if (msg[a] == squares[l][j][k]) {
						tmp[a * 3] = l;
						tmp[a * 3 + 1] = j;
						tmp[a * 3 + 2] = k;
					}
				}
			}
		}
	}
	int msg_iter = 0;
	char reoriented_digits[tmp_len + 1];
	reoriented_digits[tmp_len] = '\0';
	// re-orient digits of encoded message
	for (int j = 0; j < 3; j++)
		for (int i = j; i < tmp_len; i += 3)
			reoriented_digits[msg_iter++] = tmp[i];
	int num_groups = ceil((double)msg_len / group_size);
	int last_group_size = msg_len % group_size;
	if (last_group_size == 0)
		last_group_size = group_size;
	int layer, row, col;
	int skip, mult;
	msg_iter = 0;
	// encode message horizontally in groups of length group size
	for (int i = 0; i < num_groups; i++) {
		if (i == num_groups - 1)
			group_size = last_group_size;
		for (int j = 0; j < group_size * 3; j += 3) {
			skip = i * group_size + j;
			mult = msg_len - group_size;
			layer = reoriented_digits[skip + mult * (j / group_size)];
			row = reoriented_digits[1 + skip + mult * ((j + 1) / group_size)];
			col = reoriented_digits[2 + skip + mult * ((j + 2) / group_size)];
			msg[msg_iter++] = squares[layer][row][col];
		}
	}
	msg[msg_iter] = '\0';
	return;
}

void detrifid(char *key, int group_size, char *msg) {
	char squares[3][3][3];
	int key_len = strlen(key);
	int key_iter = 0;
	char used_letters[28]; // tracks letters already placed in squares
	for (int i = 0; i < 27; i++)
		used_letters[i] = ' '; 
	used_letters[27] = '\0';
	int used_iter = 0;
	char remaining_letters[28]; // letters unused in squares (and "+")
	int remain_iter = 0;
	char cur_key = key[0];
	// fill in squares
	for (int l = 0; l < 3; l++) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				// find next unique letter in key
				while (is_used(used_letters, cur_key) && key_iter < key_len)
					cur_key = key[++key_iter];
				// add next unique letter to current square
				if (key_iter < key_len) {
					squares[l][i][j] = cur_key;
					used_letters[used_iter++] = cur_key;
					cur_key = key[++key_iter];
				} else {
					if (key_iter == key_len) { // fill remaining_letters
						for (char c = 'a'; c <= 'z'; c++) {
							while ((is_used(used_letters, c) && c <= 'z'))
								c++;
							remaining_letters[remain_iter++] = c;
						}
						remaining_letters[remain_iter] = '+';
						remain_iter = 0;
						key_iter++;
					}
					squares[l][i][j] = remaining_letters[remain_iter++]; // add next remaining_letter to the current square
				}
			}
		}
	}
	int msg_len = strlen(msg);
	int tmp_len = msg_len * 3;
	char tmp[tmp_len + 100];
	tmp[tmp_len] = '\0';
	char cur_letter;
	// convert each letter to its indices in squares
	for (int a = 0; a < msg_len; a++) {
		for (int l = 0; l < 3; l++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					if (msg[a] == squares[l][j][k]) {
						tmp[a * 3] = l;
						tmp[a * 3 + 1] = j;
						tmp[a * 3 + 2] = k;
					}
				}
			}
		}
	}
	int num_groups = ceil((double)msg_len / group_size);
	int last_group_size = msg_len % group_size;
	if (last_group_size == 0)
		last_group_size = group_size;
	char reoriented_digits[tmp_len + 1];
	reoriented_digits[tmp_len] = '\0';
	int copy_group_size = group_size;
	int layer, row, col;
	int skip, mult;
	int msg_iter = 0;
	// decode digitized message horizontally in groups of length group size
	for (int i = 0; i < num_groups; i++) {
		if (i == num_groups - 1)
			group_size = last_group_size;
		for (int j = 0; j < group_size; j++) {
			reoriented_digits[msg_iter++] = tmp[i * copy_group_size * 3 + j];
			reoriented_digits[msg_iter++] = tmp[i * copy_group_size * 3 + copy_group_size + j];
			reoriented_digits[msg_iter++] = tmp[i * copy_group_size * 3 + 2 * copy_group_size + j];
		}
	}
	// convert decoded digits back to letters in squares
	msg_iter = 0;
	for (int i = 0; i < tmp_len; i += 3)
		msg[msg_iter++] = squares[reoriented_digits[i]][reoriented_digits[i + 1]][reoriented_digits[i + 2]];
	msg[msg_iter] = '\0';
	return;
}

void adfgx(char *polybius_key, char *transposition_key, char *msg) {
	char square[5][5];
	polybiusize(polybius_key, square); // create polybius square
	char letter_indices[5] = "adfgx"; // replace indices in encoding with adfgx
	int msg_len = strlen(msg);
	char tmp[msg_len * 2 + 1];
	char cur_letter;
	int tmp_iter = 0;
	// convert each letter to its "adfgx" indices in polybius square
	for (int a = 0; a < msg_len; a++) {
		cur_letter = msg[a];
		if (cur_letter == 'j')
			cur_letter = 'i';
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				if (cur_letter == square[i][j]) {
					tmp[tmp_iter++] = letter_indices[i];
					tmp[tmp_iter++] = letter_indices[j];
				}
			}
		}
	}
	tmp[tmp_iter] = '\0';
	columnar_transposition(transposition_key, 1, 0, "a", tmp); // simple columnar_transposition
	strcpy(msg, tmp);
	return;
}

void deadfgx(char *polybius_key, char *transposition_key, char *msg) {
	decolumnar(transposition_key, 1, 0, "a", msg); // simple decolumnar (decoding columnar_transposition)
	int msg_len = strlen(msg);
	char square[5][5];
	polybiusize(polybius_key, square); // create polybius square
	char letter_indices[5] = "adfgx"; // replace indices for decoding with adfgx
	char row_letter, col_letter;
	int row, col;
	// convert "adfgx" indices to letters in polybius square
	for (int a = 0; a < msg_len; a += 2) {
		row_letter = msg[a];
		col_letter = msg[a + 1];
		for (int i = 0; i < 5; i++) {
			if (row_letter == letter_indices[i])
				row = i;
			if (col_letter == letter_indices[i])
				col = i;
		}
		msg[a / 2] = square[row][col];
	}
	msg[msg_len / 2] = '\0';
	return;
}

void alphanum_polybiusize(char *key, char square[6][6]) {
	int key_len = strlen(key);
	int key_iter = 0;
	char used_letters[27]; // tracks letters already placed in square
	for (int i = 0; i < 26; i++)
		used_letters[i] = ' '; 
	used_letters[26] = '\0';
	int used_iter = 0;
	char remaining_letters[27]; // letters unused in square
	int remain_iter = 0;
	char cur_key = key[0];
	int is_num = -1;
	// create 6x6 square
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < 6; j++) {
			if (is_num != -1) { // add number to square
				square[i][j] = is_num + '0';
				is_num = -1;
				if (++j == 6)
					break;
			}
			// find next unique letter in key
			while (is_used(used_letters, cur_key) && key_iter < key_len)
				cur_key = key[++key_iter];
			// add next unique letter to square 
			if (key_iter < key_len) {
				square[i][j] = cur_key;
				if (cur_key <= 'j')
					is_num = (cur_key - 'a' + 1) % 10;
				used_letters[used_iter++] = cur_key;
				cur_key = key[++key_iter];
			} else { 
				if (key_iter == key_len) { // fill remaining_letters
					for (char c = 'a'; c <= 'z'; c++) {
						while (is_used(used_letters, c) && c <= 'z')
							c++;
						remaining_letters[remain_iter++] = c;
					}
					remain_iter = 0;
					key_iter++;
				}
				square[i][j] = remaining_letters[remain_iter++]; // add next remaining_letter to the square
				if (square[i][j] <= 'j') // prepare to add number to square
					is_num = (square[i][j] - 'a' + 1) % 10;
			}
		}
	}
	return;
}

void adfgvx(char *polybius_key, char *transposition_key, char *msg) {
	char square[6][6];
	alphanum_polybiusize(polybius_key, square); // create 6x6 square
	char letter_indices[6] = "adfgvx"; // replace indices in encoding with adfgvx
	int msg_len = strlen(msg); 
	char tmp[msg_len * 2 + 1];
	int tmp_iter = 0;
	// convert each letter or number to its "adfgvx" indices in 6x6 square
	for (int a = 0; a < msg_len; a++) {
		for (int i = 0; i < 6; i++) {
			for (int j = 0; j < 6; j++) {
				if (msg[a] == square[i][j]) {
					tmp[tmp_iter++] = letter_indices[i];
					tmp[tmp_iter++] = letter_indices[j];
				}
			}
		}
	}
	tmp[tmp_iter] = '\0';
	columnar_transposition(transposition_key, 1, 0, "a", tmp); // simple columnar_transposition
	strcpy(msg, tmp);
	return;
}

void deadfgvx(char *polybius_key, char *transposition_key, char *msg) {
	decolumnar(transposition_key, 1, 0, "a", msg); // simple decolumnar (decoding columnar_transposition)
	int msg_len = strlen(msg);
	char square[6][6];
	alphanum_polybiusize(polybius_key, square); // create 6x6 square
	char letter_indices[6] = "adfgvx"; // replace indices for decoding with adfgvx
	char row_letter, col_letter;
	int row, col;
	int a = 0;
	// convert "adfgvx" indices to letters or numbers in polybius square
	for (; a < msg_len; a += 2) {
		row_letter = msg[a];
		col_letter = msg[a + 1];
		for (int i = 0; i < 6; i++) {
			if (row_letter == letter_indices[i])
				row = i;
			if (col_letter == letter_indices[i])
				col = i;
		}
		msg[a / 2] = square[row][col];
	}
	msg[a / 2] = '\0';
	return;
}

// before encoding/decoding, remove spaces/non-alphanumeric characters from message and convert to all lower case 
void preprocessor(char *msg) {
	int num_spaces = 0;
	int msg_len = strlen(msg);
	int num_cut_chars = 0;
	for (int i = 0; i < msg_len; i++) {
		char cur_char = msg[i];
		if (cur_char >= 'A' && cur_char <= 'Z')
			cur_char += 'a' - 'A';
		if ((cur_char >= 'a' && cur_char <= 'z') ||
			(cur_char >= '0' && cur_char <= '9'))
			msg[i - num_cut_chars] = cur_char;
		else
			num_cut_chars++;
	}
	msg[msg_len - num_cut_chars] = '\0';
	return;
}

// after encoding/decoding, convert to all caps
void postprocessor(char *msg) {
	int msg_len = strlen(msg);
	char cur_char;
	for (int i = 0; i < msg_len; i++) {
		cur_char = msg[i];
		if ((cur_char >= 'a' && cur_char <= 'z') ||
			(cur_char >= 'A' && cur_char <= 'Z'))
			msg[i] = msg[i] += 'A' - 'a';
	}
	return;
}

int main(int argc, char *argv[]) {
	// remove all spaces/non-alphanumeric characters and convert to lower case
	char msg[strlen(argv[1]) * 3 + 1];
	strcpy(msg, argv[1]);
	preprocessor(msg);

	// call correct cipher(s) until arguments run out
	int iter = 3;
	while (iter < argc) {
		if (strcmp(argv[iter], "atbash") == 0 ||
				   strcmp(argv[iter], "deatbash") == 0) {
			atbash(msg);
			iter += 1;
		} else if (strcmp(argv[iter], "caesar") == 0) {
			caesar(atoi(argv[iter + 1]), msg);
			iter += 2;
		} else if (strcmp(argv[iter], "decaesar") == 0) {
			decaesar(atoi(argv[iter + 1]), msg);
			iter += 2;
		} else if (strcmp(argv[iter], "vigenere") == 0) {
			vigenere(argv[iter + 1], msg);
			iter += 2;
		} else if (strcmp(argv[iter], "devigenere") == 0) {
			devigenere(argv[iter + 1], msg);
			iter += 2;
		} else if (strcmp(argv[iter], "polybius_square") == 0) {
			polybius_square(argv[iter + 1], msg);
			iter += 2;
		} else if (strcmp(argv[iter], "depolybius") == 0) {
			depolybius(argv[iter + 1], msg);
			iter += 2;
		} else if (strcmp(argv[iter], "foursquare") == 0) {
			foursquare(argv[iter + 1], argv[iter + 2], msg);
			iter += 3;
		} else if (strcmp(argv[iter], "defoursquare") == 0) {
			defoursquare(argv[iter + 1], argv[iter + 2], msg);
			iter += 3;
		} else if (strcmp(argv[iter], "playfair") == 0) {
			playfair(argv[iter + 1], msg);
			iter += 2;
		} else if (strcmp(argv[iter], "deplayfair") == 0) {
			deplayfair(argv[iter + 1], msg);
			iter += 2;
		} else if (strcmp(argv[iter], "scytale") == 0) {
			scytale(atoi(argv[iter + 1]), msg);
			iter += 2;
		} else if (strcmp(argv[iter], "descytale") == 0) {
			descytale(atoi(argv[iter + 1]), msg);
			iter += 2;
		} else if (strcmp(argv[iter], "railfence") == 0) {
			railfence(atoi(argv[iter + 1]) - 1, atoi(argv[iter + 2]), msg);
			iter += 3;
		} else if (strcmp(argv[iter], "derailfence") == 0) {
			derailfence(atoi(argv[iter + 1]) - 1, atoi(argv[iter + 2]), msg);
			iter += 3;
		} else if (strcmp(argv[iter], "spiral") == 0) {
			spiral(atoi(argv[iter + 1]), atoi(argv[iter + 2]),
				   atoi(argv[iter + 3]), atoi(argv[iter + 4]),
				   atoi(argv[iter + 5]), msg);
			iter += 6;
		} else if (strcmp(argv[iter], "despiral") == 0) {
			despiral(atoi(argv[iter + 1]), atoi(argv[iter + 2]),
					 atoi(argv[iter + 3]), atoi(argv[iter + 4]),
					 atoi(argv[iter + 5]), msg);
			iter += 6;
		} else if (strcmp(argv[iter], "columnar_transposition") == 0) {
			columnar_transposition(argv[iter + 1], atoi(argv[iter + 2]),
								   atoi(argv[iter + 3]), argv[iter + 4],
								   msg);
			iter += 5;
		} else if (strcmp(argv[iter], "decolumnar") == 0) {
			decolumnar(argv[iter + 1], atoi(argv[iter + 2]), atoi(argv[iter + 3]),
					   argv[iter + 4], msg);
			iter += 5;
		} else if (strcmp(argv[iter], "bifid") == 0) {
			bifid(argv[iter + 1], msg);
			iter += 2;
		} else if (strcmp(argv[iter], "debifid") == 0) {
			debifid(argv[iter + 1], msg);
			iter += 2;
		} else if (strcmp(argv[iter], "trifid") == 0) {
			trifid(argv[iter + 1], atoi(argv[iter + 2]), msg);
			iter += 3;
		} else if (strcmp(argv[iter], "detrifid") == 0) {
			detrifid(argv[iter + 1], atoi(argv[iter + 2]), msg);
			iter += 3;
		} else if (strcmp(argv[iter], "adfgx") == 0) {
			adfgx(argv[iter + 1], argv[iter + 2], msg);
			iter += 3;
		} else if (strcmp(argv[iter], "deadfgx") == 0) {
			deadfgx(argv[iter + 1], argv[iter + 2], msg);
			iter += 3;
		} else if (strcmp(argv[iter], "adfgvx") == 0) {
			adfgvx(argv[iter + 1], argv[iter + 2], msg);
			iter += 3;
		} else if (strcmp(argv[iter], "deadfgvx") == 0) {
			deadfgvx(argv[iter + 1], argv[iter + 2], msg);
			iter += 3;
		} else {
			printf("usage: <msg> <is_capitalized>\n"
				   "    ...<cipher_name> [args]...\n"
				   "          <cipher_name> [args]:\n"
				   "             \"atbash\"\n"
				   "             \"caesar\" <key>\n"
				   "             \"decaesar\" <key>\n"
				   "             \"vigenere\" <key>\n"
				   "             \"devigenere\" <key>\n"
				   "             \"polybius_square\" <key>\n"
				   "             \"depolybius\" <key>\n"
				   "             \"foursquare\" <key_one> <key_two>\n"
				   "             \"defoursquare\" <key_one> <key_two>\n"
				   "             \"playfair\" <key>\n"
				   "             \"deplayfair\" <key>\n"
				   "             \"scytale\" <height>\n"
				   "             \"descytale\" <height>\n"
				   "             \"railfence\" <height> <direction>\n"
				   "             \"derailfence\" <height <direction>\n"
				   "             \"spiral\" <rows> <cols> <layout_direction> <corner> <clockwise>\n"
				   "             \"despiral\" <rows> <cols> <layout_direction> <corner> <clockwise>\n"
				   "             \"columnar_transposition\" <keys> <num_transpositions> <is_myszkowski> <disruption_key>\n"
				   "             \"decolumnar\" <keys> <num_transpositions> <is_myszkowski> <disruption_key>\n"
				   "             \"bifid\" <key>\n"
				   "             \"debifid\" <key>\n"
				   "             \"trifid\" <key> <group_size>\n"
				   "             \"detrifid\" <key> <group_size>\n"
				   "             \"adfgx\" <polybius_key> <transposition_key>\n"
				   "             \"deadfgx\" <polybius_key> <transposition_key>\n"
				   "             \"adfgvx\" <polybius_key> <transposition_key>\n"
				   "             \"deadfgvx\" <polybius_key> <transposition_key>\n");

			exit(0);
		}
	}
	// if second argument is not 0, capitalize message before printing result
	if (atoi(argv[2]))
		postprocessor(msg);
	printf("%s \n", msg);
	return 0;
}
