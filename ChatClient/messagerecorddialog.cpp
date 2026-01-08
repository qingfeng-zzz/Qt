{
    return QString("[%1] 【公共】 %2：%3").arg(time, sender, content);
}
else if (type == "private")
{
    return QString("[%1] 【私聊】 %2 → %3：%4").arg(time, sender, receiver.isEmpty() ? "未知" : receiver, content);
}
else
{
    return QString("[%1] 【%2】 %3：%4").arg(time, type, sender, content);
}
}
