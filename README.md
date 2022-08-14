# ECE141b -- Spring 2022 Final
## Altering a table
<hr>

We've all worked hard this term, to build a working database. Your last task, and an essential ingredient of effective software design, is to deal with change. For this task, you will implement two variations of the `alter table |name| ...` command, which changes an entity, and updates the associated row column(s) in your table. There are two variations that you'll deal with:

```
ALTER TABLE Books ADD  pub_year int;  -- adding a new column
ALTER TABLE Books DROP subtitle;  -- dropping an existing column
```

## Step 1 - Setting up your project

As usual, copy all your files from PA-10 (or the most recent working version), into this your final project folder. Be sure not to overwrite the following files:

```
makefile 
main.cpp
TestAutomatic.hpp
```


## Step 2 - 

### 2A: Create a new "AlterStatement" class to your system

In order to support the `alter table...` you'll need to create a new class to handle that statement type. Choose whatever name you like. 

### 2B: Add code to implement the "Alter table" command(s)

Handling the `ALTER TABLE` command also requires that you add code to one of your controller classes, typically `Database.hpp`. Call this method when it's time to execute an `AlterTable...` statement.

A typical scenario looks like this when a user issues an `ALTER` command. Don't forget to output the "Query OK..." information:

```
ALTER TABLE Books add pub_year varchar(4);  
Query Ok, 20 rows affected (0.00123 secs)
```


## Step 3 - Developing your solution

An `ALTER TABLE` command is capable of making three types of changes to an entity: 1) ADD column; 2) DROP column; 3) MODIFY existing column. For your final, you are ONLY asked to alter your table using ADD column and DROP column.  Also, you won't be asked to change the primary key for a table, so your indexes will not be affected.

The key to understanding this challenge, is to recognize that the `ALTER TABLE` command actually affects two things. First, it changes the definition of a known `Entity`, by adding or removing a new attribute.  Second, it changes _every_ existing row in the table associated with this `Entity`, and adds a new (empty) value for the newly added attribute or removes an existing value from the row. 

You'll likely be making the following changes to your code:

1. Create a new `AlterStatement` class, as you've done before
2. Add support for this class in one of your `Processor` classes, as you've done before
3. Add logic in a controller class (e.g. `Database`) to impelement this command
4. Add methods to `Entity` to add/remove an `Attribute`
5. Re-save the `Entity` when it changes
6. Update all rows associated with the changed `Entity` to add/remove a data column

If you think about this -- you'll see that you have already created many tools that can help.


> **DON'T PANIC:** -- if this is overwhelming, just do your best. A working solution is a great result. But if you can't accomplish that, you can still get partial credit. Work methodically, and follow the patterns you've been using throughout the term. Remember that every new command requires that we have a `Statement` (for parsing and state management), and a corresponding method to handle the command in one of the `CmdProcessor` classes (`SQLProcessor`, `DBProcessor`). Work iteratively on this, and make sure to leave comments in your code if you can't finish a task, so we can follow the logic you were trying to implement.

### Changing your `Entity`

To change your entity, load it from storage (or use the one you have in memory if it's already loaded).  Add or remove the specified attribute, and resave the entity to storage, using its original block_number. 

### Changing your rows

**NOTE:** It's very likely you already get most of the code you need to do this work, from the commands you've already implemented. 

For example, you can conveniently get a list of `Rows` by invoking `select * from table`. For each element in your collection, add or remove the given field. Then re-save the `Row` to storage, as if you were doing and `update` to its original block_number.  

## Submitting your final solution

You can resubmit your code to github as often as you like during the final to help with testing. The `TestAutomatic.hpp` file provides a new test called `doAlterTest` for validating your `ALTER TABLE...` commands.

#### Grading 
```
Compile test -- 10pts
AddColumn test -- 45pts (ALTER TABLE statement)
RemoveColumn test -- 45pts (ALTER TABLE statement)
``` 

Make sure to update the `AboutMe.cpp` file with your student details. Then commit your changes to github. 

Good luck everyone -- it's been delightful to work with each you this term.
