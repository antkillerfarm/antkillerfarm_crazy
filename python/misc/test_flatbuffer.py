import flatbuffers

from school import School
from school import Student

builder = flatbuffers.Builder(1024)

stu1 = builder.CreateString('zhang3')
stu2 = builder.CreateString('li4')

Student.StudentStart(builder)
Student.StudentAddName(builder, stu1)
Student.StudentAddNumber(builder, 3)
zhang3 = Student.StudentEnd(builder)

Student.StudentStart(builder)
Student.StudentAddName(builder, stu2)
Student.StudentAddNumber(builder, 4)
li4 = Student.StudentEnd(builder)

School.SchoolStartStudentsVector(builder, 2)
builder.PrependUOffsetTRelative(li4)
builder.PrependUOffsetTRelative(zhang3)
students = builder.EndVector(2)

School.SchoolStart(builder)
School.SchoolAddStudents(builder, students)
school_result = School.SchoolEnd(builder)

builder.Finish(school_result)

buf = builder.Output()

new_school = School.School.GetRootAsSchool(buf, 0)
print(new_school.Students(0).Name())
print(new_school.Students(0).Number())
print(new_school.Students(1).Name())
print(new_school.Students(1).Number())

with open("school.bin", mode="wb") as f:
    f.write(buf)
